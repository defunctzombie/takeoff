#pragma once

#include <QString>

class Scale
{
    public:
        Scale(float drawing = 1, float actual = 1) :
            _drawing(drawing), _actual(actual)
        {
            if (_drawing < 1)
            {
                int base = 1;    
                while (drawing - int(drawing) > 0)
                {
                    drawing *= 10;
                    base *= 10;
                }
                
                const int num = int(drawing);
                const int div = gcd(num, base);
                
                if (div != 0)
                    _drawingFrac = QString("%1/%2\"").arg(num/div).arg(base/div);
                else
                    _drawingFrac = "INVALID";
            }
            else
            {
                _drawingFrac = QString("%1\"").arg(_drawing);
            }
        }
        
        /** @return the number of feel in one drawing inch */
        float scale() const
        {
            return 1/_drawing * _actual;
        }
        
        QString toString() const
        {
            return _drawingFrac + QString(" = %1'").arg(_actual);
            //return "Custom Scale";
        }
        
    private:
        float _drawing;
        float _actual;
        
        QString _drawingFrac;
        
        static int gcd(int a, int b)
        {
            if (b == 0)
                return a;
            else
                return gcd(b, a % b);
        }
};