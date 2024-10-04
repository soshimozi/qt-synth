

#include <qobject.h>

QString formatNumberPrefix(double number) {

    QString suffix;
    int digits = 0;
    if (std::abs(number) >= 1e9) {
        number /= 1e9;
        suffix = "G";
        digits = 1;
    } else if (std::abs(number) >= 1e6) {
        number /= 1e6;
        suffix = "M";
        digits = 1;
    } else if (std::abs(number) >= 1e3) {
        number /= 1e3;
        suffix = "K";
        digits = 1;
    }


    return QString::number(number, 'f', digits) + suffix;
}
