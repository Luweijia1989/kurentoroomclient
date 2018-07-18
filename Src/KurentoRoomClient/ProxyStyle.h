#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

class cProxyStyle : public QProxyStyle
{
    Q_OBJECT
public:
	explicit cProxyStyle();

protected:
    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption * option,
                                  QPainter * painter, const QWidget * widget = 0) const;

signals:

public slots:

};

#endif // PROXYSTYLE_H
