#pragma once

#include <QString>

struct NaturalSort
{
    bool operator()(const QString& first,const QString& second) {

        QString::ConstIterator f = first.begin();
        QString::ConstIterator s = second.begin();

        // this doesn't work because 1 < 2, need to grab 10
        // a1, a10, a2
        // if there are more digits after the current we need to grab all digits
        while (f != first.end() || s != second.end())
        {
            const QChar& c1 = *f;
            const QChar& c2 = *s;

            // if neither are a digit then normal comparison
            if (!c1.isDigit() || !c2.isDigit()) {
                if (c1 != c2) {
                    return c1 < c2;
                }

                // characters are the same, go to next character
                f++;
                s++;
                continue;
            }

            QString num1(c1);
            QString num2(c2);

            // while !end and have digit
            while (f != first.end()) {
                if (!f->isDigit()) {
                    break;
                }

                num1 += *f;
                f++;
            }

            while (s != second.end()) {
                if (!s->isDigit()) {
                    break;
                }

                num2 += *s;
                s++;
            }

            return num1.toInt() < num2.toInt();
        }

        return first < second;
    }
};

