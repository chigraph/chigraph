#include "qmodeldatalistdecoder.h"

#include <QtCore/QMimeData>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>

// Be less strict about unserializable values that are part of the stream.
class QLousyVariantDecoder
{
public:
    class QVariantExt : public QVariant {
    public:
        inline void createProxy(int typeId) { create(typeId, Q_NULLPTR); }
    };

    quint32     metaType;
    qint8       isNull;
    QVariantExt variant ;
    QByteArray  userType;
    bool        isLoaded;
};

class QModelDataListDecoderPrivate
{
public:
    QHash<QPair<int, int>, QMap<int, QLousyVariantDecoder> > m_Data;
};

QDebug operator<<(QDebug debug, const QLousyVariantDecoder &v)
{
    return debug << QStringLiteral("<<<")
        << QStringLiteral("Type:"      ) << v.metaType << v.userType
        << QStringLiteral(", Is NULL:" ) << v.isNull
        << QStringLiteral(", Is valid:") << v.isLoaded
        << QStringLiteral(", Value:"   ) << v.variant
        << QStringLiteral(">>>");
}

QDataStream &operator<<(QDataStream &s, const QLousyVariantDecoder &self)
{
    return s << self.variant;
}

QDataStream &operator>>(QDataStream &s, QLousyVariantDecoder &self)
{
    // There is no Qt ways to doing this before 5.7 beside creating it by hand
    // Qt5.7 support transactions, but replicating the exact behavior of the
    // following code is longer than not using transactions.
    s >> self.metaType;
    s >> self.isNull;

    if (self.metaType == QVariant::UserType) {
        s >> self.userType;
        self.metaType = QMetaType::type(self.userType.constData());

        if (self.metaType == QMetaType::UnknownType) {
            s.setStatus(QDataStream::ReadCorruptData);
            return s;
        }
    }
    else
        self.userType = QMetaType::typeName(self.metaType);

    if (!self.isNull) {
        self.variant.createProxy(self.metaType);

        void* data = const_cast<void *>(self.variant.constData());

        // Ignore errors, as the way it is implemented, the field is empty,
        // so the streams remains valid. However dropping the data wont work.
        self.isLoaded = QMetaType::load(s, self.variant.type(), data);
    }

    return s;
}

// hack to execute code at a RANDOM moment during initialization.
static auto _DUMMY = ([]()->bool {
    qRegisterMetaType               <QLousyVariantDecoder>("QLousyVariantDecoder");
    qRegisterMetaTypeStreamOperators<QLousyVariantDecoder>("QLousyVariantDecoder");
    return true;
})();

QModelDataListDecoder::QModelDataListDecoder(const QMimeData* data)
    : d_ptr(new QModelDataListDecoderPrivate)
{
    if (!data)
        return;

    // Check all payloads if one can be converted to the right QMetaType
    auto buf = data->data("application/x-qabstractitemmodeldatalist");

    if (buf.isEmpty())
        return;

    QDataStream s(buf);

    while (!s.atEnd()) {
        int r, c;
        QMap<int, QLousyVariantDecoder> v;
        s >> r >> c >> v;

        // only add valid items
        if (r+1 && c+1)
            d_ptr->m_Data[{r, c}] = std::move(v);
    }
}

QModelDataListDecoder::~QModelDataListDecoder()
{
    delete d_ptr;
}

QPair<int, int> QModelDataListDecoder::firstElement() const
{
    if (d_ptr->m_Data.isEmpty())
        return {-1,-1};

    return d_ptr->m_Data.begin().key();
}

bool QModelDataListDecoder::canConvert(quint32 typeId, int role, int row, int col) const
{
    auto v = data(role, row, col);

    if (v.isValid())
        return v.canConvert(typeId);

    const auto pair = (row+1&&col+1) ? QPair<int,int>(row, col) : firstElement();

    if (!d_ptr->m_Data.contains(pair))
        return false;

    auto info = d_ptr->m_Data[pair][role];

    if (info.metaType == typeId)
        return true;

    QLousyVariantDecoder::QVariantExt var;
    var.createProxy(info.metaType);

    return var.canConvert(typeId);;
}

QVariant QModelDataListDecoder::data(int role, int row, int col) const
{
    const auto pair = (row+1&&col+1) ? QPair<int,int>(row, col) : firstElement();

    if (!d_ptr->m_Data.contains(pair))
        return {};

    return d_ptr->m_Data[pair][role].variant;
}

quint32 QModelDataListDecoder::typeId(int role, int row, int col) const
{
    const auto pair = (row+1&&col+1) ? QPair<int,int>(row, col) : firstElement();

    if (!d_ptr->m_Data.contains(pair))
        return QMetaType::UnknownType;

    const auto data = d_ptr->m_Data[pair];

    if (!data.contains(role))
        return QMetaType::UnknownType;

    return data[role].metaType;
}
