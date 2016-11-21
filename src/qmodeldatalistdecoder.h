#pragma once

#include <QtCore/QVariant>
#include <QtCore/QPair>

class QMimeData;

class QModelDataListDecoderPrivate;

/*
 * The default model implementation adds the `application/x-qabstractitemmodeldatalist`
 * MIME Type. This things is totally undocumented, but some reverse engineering
 * of it's encoder indicate it is (as of Qt 5.6) an array of:
 *
 *     Tuple<int, int, QMap<int, QVaritant>>
 *
 * pushed in a `QDataStream`. This format suck, as it's not really able to
 * express the source `QModelIndex`. However, it does contain the QVariant
 * of some of its role. From them, even the invalid ones, the QMetaType id
 * should be pushed into the stream. It should have been easy, but there
 * is another problem. QVariant::load exits early when decoding a QMetaType
 * that cannot be encoder. While it prints the QMetaType on stderr, it doesn't
 * export it. This, in turn, causes another problem where the QMap will be empty
 * if a single element fail to be deserialized. This little class implements a
 * serializable Qt type that mimics the QVariant decoder to be able to extract
 * the correct type.
 *
 * The QVariant data is encoded as (it is stable and documented):
 *
 *  * The type of the data (quint32)
 *  * The null flag (qint8)
 *  * The data of the specified type
 * 
 * Reference:
 *
 *  * http://doc.qt.io/qt-5/datastreamformat.html
 *  * qvariant.cpp
 *  * qabstractitemmodel.cpp
 */
class QModelDataListDecoder
{
public:
    explicit QModelDataListDecoder(const QMimeData* data);
    virtual ~QModelDataListDecoder();

    bool canConvert(quint32 typeId, int role = Qt::EditRole, int row = -1, int column = -1) const;

    template<typename T>
    bool canConvert(int role = Qt::EditRole) const {
        return canConvert(qMetaTypeId<T>(), role);
    }

    QVariant data(int role, int row = -1, int column = -1) const;

    int count();

    QPair<int, int> firstElement() const;

    quint32 typeId(int role = Qt::EditRole, int row = -1, int column = -1) const;

private:
    QModelDataListDecoderPrivate* d_ptr;
};

