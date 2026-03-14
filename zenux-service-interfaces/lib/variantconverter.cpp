#include "variantconverter.h"

namespace Zera {

QVariant VariantConverter::returnInt(const QString &message)
{
    return QVariant(message.toInt());
}

QVariant VariantConverter::returnString(const QString &message)
{
    return QVariant(message);
}

QVariant VariantConverter::returnStringList(const QString &message)
{
    return QVariant(message.split(";"));
}

QVariant VariantConverter::returnDouble(const QString &message)
{
    return QVariant(message.toDouble());
}

QVariant VariantConverter::returnBool(const QString &message)
{
    return QVariant(message.toInt() == 1);
}

}
