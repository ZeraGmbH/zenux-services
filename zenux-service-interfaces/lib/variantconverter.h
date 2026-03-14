#ifndef VARIANTCONVERTER_H
#define VARIANTCONVERTER_H

#include <QVariant>

namespace Zera {

class VariantConverter
{
public:
    static QVariant returnInt(const QString &message);
    static QVariant returnString(const QString &message);
    static QVariant returnStringList(const QString &message);
    static QVariant returnDouble(const QString &message);
    static QVariant returnBool(const QString &message);
};

}
#endif // VARIANTCONVERTER_H
