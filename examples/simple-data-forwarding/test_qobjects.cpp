#include "test_qobjects.hpp"

QString testnode1::getTXT() const
{
	return m_txt;
}

void testnode1::setTXT(QString arg)
{
	if (m_txt == arg)
		return;

	m_txt = arg;
	emit txtChanged(arg);
}
