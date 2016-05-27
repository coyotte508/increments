#include "utils.h"

uint qHash( const QVariant & var )
{
    if ( !var.isValid() || var.isNull() )
        //return -1;
        Q_ASSERT(0);

    switch ( var.type() )
    {
    case QVariant::Int:
        return qHash( var.toInt() );
        break;
    case QVariant::UInt:
        return qHash( var.toUInt() );
        break;
    case QVariant::Bool:
        return qHash( var.toUInt() );
        break;
    case QVariant::Double:
        return qHash( var.toUInt() );
        break;
    case QVariant::LongLong:
        return qHash( var.toLongLong() );
        break;
    case QVariant::ULongLong:
        return qHash( var.toULongLong() );
        break;
    case QVariant::String:
        return qHash( var.toString() );
        break;
    case QVariant::Char:
        return qHash( var.toChar() );
        break;
    case QVariant::StringList:
        return qHash( var.toString() );
        break;
    case QVariant::ByteArray:
        return qHash( var.toByteArray() );
        break;
    case QVariant::Date:
    case QVariant::Time:
    case QVariant::DateTime:
    case QVariant::Url:
    case QVariant::Locale:
    case QVariant::RegExp:
        return qHash( var.toString() );
        break;
    case QVariant::Map:
    case QVariant::List:
    case QVariant::BitArray:
    case QVariant::Size:
    case QVariant::SizeF:
    case QVariant::Rect:
    case QVariant::LineF:
    case QVariant::Line:
    case QVariant::RectF:
    case QVariant::Point:
    case QVariant::PointF:
        // not supported yet
        break;
    case QVariant::UserType:
    case QVariant::Invalid:
    default:
        return -1;
    }

    // could not generate a hash for the given variant
    return -1;
}

QList<int> randomClique(int c, int l)
{
    QList<int> ret;

    std::uniform_int_distribution<> f(0,l-1);
    for (int i = 0; i < c; i++) {
        ret.push_back(f(e1));
    }

    return ret;
}

QString debug(const QMap<int, int> &histo)
{
    QString ret;
    ret += "{";
    for (int x: histo.keys()) {
        if (ret.size() > 1) {
            ret += ", ";
        }
        ret += "\"" + QString::number(x) + "\"" + ": " + QString::number(histo[x]);
    }
    ret += "}";
    return ret;
}