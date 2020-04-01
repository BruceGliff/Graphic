
template<int Dim>
Vector_base<Dim>::Vector_base(std::initializer_list<float> const & list)  noexcept
{
    int i = 0;
    std::for_each(list.begin(), list.end(), [this, &i](float x)
    {
        data_[i] = x;
        ++i;
    });
}

template<int Dim>
Vector_base<Dim>::Vector_base(Vector_base const & vec) noexcept
{
    std::copy(vec.begin(), vec.end(), begin());
}

template<int Dim>
Vector_base<Dim> & Vector_base<Dim>::operator=  (Vector_base const & vec) noexcept
{
    std::copy(vec.begin(), vec.end(), begin());
    return *this;
}

template<int Dim>
Vector<Dim> Vector_base<Dim>::operator-()const noexcept 
{ 
    Vector<Dim> tmp;
    int i = 0;
    std::for_each(begin(), end(), [&tmp, &i](float x)
    {
        tmp[i] = -x;
        ++i;
    });
    return tmp;
}

template<int Dim>
Vector<Dim> Vector_base<Dim>::operator*(float const k) const noexcept 
{ 
    Vector<Dim> tmp;
    int i = 0;
    std::for_each(begin(), end(), [&tmp, &i, k](float x)
    {
        tmp[i] = x * k;
        ++i;
    });
    return tmp;
}

template<int Dim>
Vector<Dim> Vector_base<Dim>::operator+(Vector_base const & vec) const noexcept
{
    Vector<Dim> tmp;
    int i = 0;
    std::for_each(begin(), end(), [&vec, &tmp, &i](float x)
    {
        tmp[i] = x + vec[i];
        ++i;
    });
    return tmp;
}

template<int Dim>
Vector<Dim> Vector_base<Dim>::operator-(Vector_base const & vec) const noexcept
{
    Vector<Dim> tmp;
    int i = 0;
    std::for_each(begin(), end(), [&vec, &tmp, &i](float x)
    {
        tmp[i] = x - vec[i];
        ++i;
    });
    return tmp;
}

template<int Dim>
float Vector_base<Dim>::dot(Vector_base const & vec) const noexcept
{
    float tmp = 0;
    int i = 0;
    std::for_each(begin(), end(), [&vec, &tmp, &i](float x)
    {
        tmp += x * vec[i];
        ++i;
    });
    return tmp;
}

template<int Dim>
float Vector_base<Dim>::length() const noexcept
{
    return std::sqrt(dot(*this));
}

template <int Dim>
std::ostream & operator<<(std::ostream & os, Vector_base<Dim> const & vec)
{
    for (int i = 0; i != Dim; ++i)
        os << vec[i] << ' ';
    return os;
}

template<int Dim>
Matrix<Dim>::Matrix(std::initializer_list<float> const & list) noexcept
{
    std::copy(list.begin(), list.end(), data_);
}

template<int Dim>
Matrix<Dim>::Matrix(Matrix const & mat) noexcept
{ 
    std::copy(mat.data_, mat.data_ + Dim * Dim, data_); 

} 

template<int Dim>
Vector<Dim> Matrix<Dim>::operator*(Vector<Dim> const & vec) const noexcept
{
    Vector<Dim> out;

    for(int i = 0; i != Dim; ++i)
        for(int j = 0; j != Dim; ++j)
            out[i] += data_[i * Dim + j] * vec[j];
    return out;
}


template<int Dim>
std::ostream & operator<<(std::ostream & os, Matrix<Dim> const & mat)
{
    for (int i = 0; i != Dim; ++i)
    {
        for (int  j = 0; j != Dim; ++j)
            os << mat[i][j] << ' ';
        os << '\n';
    }
    return os;
}