#ifndef __TEST_QOBJECTS_HPP__AF779D98_80FC_4E47_BAC8_C919FE80A131
#define __TEST_QOBJECTS_HPP__AF779D98_80FC_4E47_BAC8_C919FE80A131

#include <qobject.h>

class testnode1: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString txt READ getTXT WRITE setTXT NOTIFY txtChanged USER true)
public:
	QString getTXT() const;
public slots:
	void setTXT(QString arg);
signals:
	void txtChanged(QString arg);
private:
	QString m_txt;
};

#endif /* __TEST_QOBJECTS_HPP__AF779D98_80FC_4E47_BAC8_C919FE80A131 */

