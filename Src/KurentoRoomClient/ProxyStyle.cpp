#include "stdafx.h"
#include "ProxyStyle.h"


cProxyStyle::cProxyStyle() :
    QProxyStyle()
{
}

void cProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption * option,
                                QPainter * painter, const QWidget * widget) const
{
    if (PE_FrameFocusRect == element)
    {
        /// do not draw focus rectangle
    }
    else
    {
        QProxyStyle::drawPrimitive(element, option,painter, widget);
    }
}
