/* See LICENSE file for copyright and license details. */

#ifndef __GRAPHICSNODEDEFS_HPP__49761BBD_1BA5_49AC_8C23_88079EED41F1
#define __GRAPHICSNODEDEFS_HPP__49761BBD_1BA5_49AC_8C23_88079EED41F1

#include <QGraphicsItem>

enum GraphicsNodeItemTypes {
	TypeNode = QGraphicsItem::UserType + 1,
	TypeBezierEdge = QGraphicsItem::UserType + 2,
	TypeSocket = QGraphicsItem::UserType + 3
};

#endif /* __GRAPHICSNODEDEFS_HPP__49761BBD_1BA5_49AC_8C23_88079EED41F1 */

