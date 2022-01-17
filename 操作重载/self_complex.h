#ifndef SELF_COMLPEX_H
#define SELF_COMPLEX_H

#include <limits>

template<class T>
typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
    almost_equal(T x, T y, int ulp=2)
{
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x-y) <= std::numeric_limits<T>::epsilon() * std::abs(x+y) * ulp
        // unless the result is subnormal
        || std::abs(x-y) < std::numeric_limits<T>::min();
}

namespace test_complex{
    class complex {
        private:
            double _M_real;
            double _M_imag;

        public:
            complex(double re=0.0, double im=0.0) : _M_real(re),_M_imag(im) {};

            complex(const complex &_M){
                _M_real = _M.getReal();
                _M_imag = _M.getImag();
            }

            double getReal() const{
                return _M_real;
            }

            double getImag() const{
                return _M_imag;
            }
    };

    // 使用自定义字面量
    complex 
    operator""_i(long double _num){
        return complex(0.0,_num);
    }

    // 重定义<<
    std::ostream& operator<<(std::ostream &os, const complex &_M_complex){
        os<<_M_complex.getReal()<<'+'<<_M_complex.getImag()<<'i';
        return os;
    }  

    // 重定义+
    complex
    operator+(const complex &m1, const complex &m2){
        double real = m1.getReal() + m2.getReal();
        double imag = m1.getImag() + m2.getImag();
        return complex(real,imag);
    }

    // 重定义==
    bool
    operator==(const complex &m1, const complex &m2){
        bool real_eq = almost_equal(m1.getReal(), m2.getReal());
        bool imag_eq = almost_equal(m1.getImag(), m2.getImag());
        return real_eq && imag_eq;
    }

    // 对称的，需要重定义!=
    bool
    operator!=(const complex &m1, const complex &m2){
        return !operator==(m1,m2);
    }

}

#endif