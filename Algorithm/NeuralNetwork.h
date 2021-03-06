#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <cmath>
#include <functional>
#include <vector>

namespace rapid
{

using ErrorType = double;

inline double sigmoid(double x)
{ return 1 / (1 + pow(M_E, -x)); }
inline double tanh(double __x)
{ return std::tanh(__x); }
inline constexpr double softsign(double __x)
{ return __x / (1 + __x > 0 ? __x : -__x); }

inline double softsign_d(double __x)
{ return 1 / pow(1 + __x > 0 ? __x : -__x, 2); }
inline double tanh_d(double __x)
{ return 1 - pow(tanh(__x), 2); }

template<typename _T = double>
inline _T output_residual(_T __output, _T __sample)
{ return -(__output - __sample) * __output * (1 - __output); }

template<typename _T = double>
inline _T hide_residual(_T __pre_residual, _T __sigmoid)
{ return -__pre_residual * __sigmoid * (1 - __sigmoid); }

template<typename _T1 = double,
         typename _T2 = _T1,
         typename _R = _T1>
inline double multiple(double x, double y)
{ return x * y; }

template<typename _T = double>
inline _T sum(_T __x, _T __y)
{ return __x + __y; }

template<typename _T = double>
inline ErrorType error(_T __1, _T __2)
{ return pow(__1 - __2, 2); }

template<typename _ResultType,
         typename ... _Args>
std::function<_ResultType(_Args...)> __Activation;
template<typename _ResultType,
         typename ... _Args>
std::function<_ResultType(_Args...)> __Multiple;
template<typename _ResultType,
         typename ... _Args>
std::function<_ResultType(_Args...)> __Sum;
template<typename _ResultType,
         typename ... _Args>
std::function<_ResultType(_Args...)> __Error;
template<typename _ResultType,
         typename ... _Args>
std::function<_ResultType(_Args...)> __OResidual; // output layer residual function
template<typename _ResultType,
         typename ... _Args>
std::function<_ResultType(_Args...)> __HResidual; // hide layer residual function

template<typename _DataType = double,
         typename _WeightType = double>
struct alignas(8) BPNeuralNetwork
{
    template<typename T>
    using VectorType = std::vector<T>;

    using DataType = _DataType;
    using DataGroup = std::vector<DataType>;
    using SizeType = unsigned long long;
    using WeightType = _WeightType;
    using WeightGroupItem = std::vector<WeightType>;
    using WeightGroup = std::vector<WeightGroupItem*>;
    using WeightData = std::vector<WeightGroup*>;

    WeightData _M_weight;
    WeightGroup _M_output_weight;
    SizeType _M_generation = 1;
    double _M_learn_efficency = 0.6;
    ErrorType _M_error_bound = 0.001;

    BPNeuralNetwork()
    {
        std::function<double(double)> f1 = std::bind(&sigmoid, std::placeholders::_1);
        set_activation(f1);
        std::function<double(double, double)> f2 =
                std::bind(&multiple<double, double, double>,
                          std::placeholders::_1,
                          std::placeholders::_2);
        set_multiple(f2);
        f2 = std::bind(&sum<double>,
                       std::placeholders::_1,
                       std::placeholders::_2);
        set_sum(f2);
        f2 = std::bind(&output_residual<double>,
                       std::placeholders::_1,
                       std::placeholders::_2);
        set_output_residual(f2);
        f2 = std::bind(&hide_residual<double>,
                       std::placeholders::_1,
                       std::placeholders::_2);
        set_hide_residual(f2);
        std::function<ErrorType(double, double)> f3 =
                std::bind(&error<double>,
                          std::placeholders::_1,
                          std::placeholders::_2);
        set_error(f3);
        set_hide_layer_number(1);
    }
    ~BPNeuralNetwork()
    { _F_clear(); }
    void clear()
    {
        _F_realloc(_M_weight, 1);
        _F_realloc(_M_output_weight);
        set_generation_number(0);
    }

    /*
     * the number of [__in] is same as [__out]'s
     * the number of each group of [__in] or [__out] are same
     */
    void train(std::initializer_list<DataType> __in,
               std::initializer_list<DataType> __out)
    {
        train(__in.begin(), __in.end(), __in.size(),
              __out.begin(), __out.end(), __out.size());
    }

    /*
     * train one group for one time. so, you should call for multiple times.
     */
    template<typename _InputIterator,
             typename _OutputIterator>
    bool train(_InputIterator __ibegin, _InputIterator __iend, SizeType __is,
               _OutputIterator __obegin, _OutputIterator __oend, SizeType __os)
    {
        if(_M_weight.size() == 0 || __is != hide_node_size())
        {
            return false;
        }
        DataGroup __input, __output;
        _F_realloc(__input, __is);
        _F_realloc(__output, __os);
        for(auto it = __ibegin; it != __iend; ++it)
        {
            __input.push_back(*it);
        }
        for(auto it = __obegin; it != __oend; ++it)
        {
            __output.push_back(*it);
        }
        _F_train(__input, __output);
        return true;
    }

    template<typename _ResultType,
             typename ... _Args>
    void set_activation(std::function<_ResultType(_Args...)> &&__f)
    { __Activation<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_activation(const std::function<_ResultType(_Args...)> &__f)
    { __Activation<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_multiple(std::function<_ResultType(_Args...)> &&__f)
    { __Multiple<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_multiple(const std::function<_ResultType(_Args...)> &__f)
    { __Multiple<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_sum(std::function<_ResultType(_Args...)> &&__f)
    { __Sum<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_sum(const std::function<_ResultType(_Args...)> &__f)
    { __Sum<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_error(std::function<_ResultType(_Args...)> &&__f)
    { __Error<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_error(const std::function<_ResultType(_Args...)> &__f)
    { __Error<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_output_residual(std::function<_ResultType(_Args...)> &&__f)
    { __OResidual<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_output_residual(const std::function<_ResultType(_Args...)> &__f)
    { __OResidual<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_hide_residual(std::function<_ResultType(_Args...)> &&__f)
    { __HResidual<_ResultType, _Args...> = __f; }

    template<typename _ResultType,
             typename ... _Args>
    void set_hide_residual(const std::function<_ResultType(_Args...)> &__f)
    { __HResidual<_ResultType, _Args...> = __f; }

    void set_learning_efficency(double __e)
    { _M_learn_efficency = __e; }

    void set_hide_layer_number(SizeType __s)
    {
        if(__s == 0) return;
        if(_M_weight.size() < __s)
        {
            _M_weight.resize(__s);
        }
        else
        {
            WeightData temp(_M_weight.begin(), _M_weight.begin() + __s);
            _M_weight.swap(temp);
        }
    }
    // param[__s]: 0--auto
    void set_generation_number(SizeType __s)
    { _M_generation = __s; }

    // param[__layer]: index begin at 0
    void set_weight(SizeType __layer,
                    std::initializer_list<std::initializer_list<WeightType>> __w)
    { set_weight(__layer, __w.begin(), __w.end(), __w.size()); }

    // param[__layer]: index begin at 0
    template<typename _InputIterator>
    void set_weight(SizeType __layer,
                    _InputIterator __begin,
                    _InputIterator __end,
                    SizeType __s)
    {
        SizeType __last = _M_weight.size();
        if(_M_weight.size() <= __layer)
        {
            _M_weight.resize(__layer + 1);
        }
        while(__last <= __layer)
        {
            _M_weight[__last++] = new WeightGroup(__s);
        }
        if(_M_weight[__layer] == nullptr)
        {
            _M_weight[__layer] = new WeightGroup(__s);
        }
        _F_set_weight(*(_M_weight[__layer]), __begin, __end, __s);
    }
    void set_output_weight(std::initializer_list<std::initializer_list<WeightType>> __w)
    { set_output_weight(__w.begin(), __w.end(), __w.size()); }
    template<typename _InputIterator>
    void set_output_weight(_InputIterator __begin, _InputIterator __end, SizeType __s)
    { _F_set_weight(_M_output_weight, __begin, __end, __s); }

    // return size of the layer [hide]
    SizeType layer_size() const
    { return _M_weight.capacity(); }
    SizeType hide_node_size() const
    { return _M_weight.at(0)->at(0)->capacity(); }
    SizeType output_size() const
    { return _M_output_weight.at(0)->capacity(); }

    DataGroup predict(std::initializer_list<DataType> __d) const
    { return predict(__d.begin(), __d.end()); }
    template<typename _InputIterator>
    DataGroup predict(_InputIterator __begin, _InputIterator = _InputIterator()) const
    {
        DataGroup __input(hide_node_size()), __output(output_size());
        for(SizeType __i = 0; __i < hide_node_size(); ++__i, ++__begin)
        {
            __input[__i] = *__begin;
        }
        _F_input_to_output(__input, __output);
        return __output;
    }
    void _F_input_to_output(DataGroup &__in, DataGroup &__out) const
    {
        DataGroup __temp(hide_node_size());
        _F_input_to_hide(__in, __temp);
        _F_hide_to_output(__temp, __out);
    }
    void _F_input_to_hide(DataGroup &__in, DataGroup &__out) const
    {
        if(__out.capacity() != hide_node_size())
        {
            return;
        }
        DataGroup __temp(hide_node_size());
        DataGroup *__data[2] = {&__in, &__temp};
        SizeType i;
        for(i = 0; i < layer_size(); ++i)
        {
            DataGroup &__this = *__data[i % 2];
            DataGroup &__next = *__data[(i + 1) % 2];
            _F_layer_forward(__this, hide_node_size(), __next, i, hide_node_size());
        }
        __out.swap(*__data[i % 2]);
    }

    void _F_hide_to_output(const DataGroup &__h, DataGroup &__o) const
    {
        if(__o.capacity() != output_size())
        {
            return;
        }
        _F_layer_forward(__h, hide_node_size(), __o, layer_size(), output_size());
    }

    void _F_clear()
    {
        _F_realloc(_M_weight);
        _F_realloc(_M_output_weight);
        set_generation_number(0);
    }
    /* get the weight
     * param[__layer]:  the .no of layer
     * param[__item]:  the .no of node
     * param[__index]:  the .no of weight array
     */
    WeightType _F_weight(SizeType __layer, SizeType __item, SizeType __index) const
    {
        if(__layer >= layer_size())
        {
            return _M_output_weight.at(__item)->at(__index);
        }
        return _M_weight.at(__layer)->at(__item)->at(__index);
    }
    /* calculate the weighted sum
     * param[__in]:  to be calculated
     * param[__in_size]:  the size of [__in]
     * param[__value]:  __out__  calculation results
     * param[__layer]:  the layer position of the data
     * param[__item_index]:  the .no of the [__value]
     */
    void _F_get_weighted_sum_forward(const DataGroup &__in,
                                     SizeType __in_size,
                                     DataType &__value, // __out__
                                     SizeType __layer,
                                     SizeType __item_index) const
    {
        __value = __Multiple<DataType, DataType, WeightType>(
                      __in[0],
                      _F_weight(__layer, 0, __item_index));
        for(SizeType n = 1; n < __in_size; ++n)
        {
            __value = __Sum<DataType, DataType, DataType>(
                          __value,
                          __Multiple<DataType, DataType, WeightType>(
                              __in[n],
                              _F_weight(__layer, n, __item_index)));
        }
    }
    /* calculate the weighted sum
     * param[__in]:  to be calculated
     * param[__in_size]:  the size of [__in]
     * param[__value]:  __out__  calculation results
     * param[__layer]:  the layer position of the data
     * param[__item_index]:  the .no of the [__value]
     */
    void _F_get_weighted_sum_backward(const DataGroup &__in,
                                      SizeType __in_size,
                                      DataType &__value, // __out__
                                      SizeType __layer,
                                      SizeType __item_index) const
    {
        __value = __Multiple<DataType, DataType, WeightType>(
                      __in[0],
                      _F_weight(__layer, __item_index, 0));
        for(SizeType n = 1; n < __in_size; ++n)
        {
            __value = __Sum<DataType, DataType, DataType>(
                          __value,
                          __Multiple<DataType, DataType, WeightType>(
                              __in[n],
                              _F_weight(__layer, __item_index, n)));
        }
    }
    void _F_layer_forward(const DataGroup &__in,
                          SizeType __in_size,
                          DataGroup &__out,
                          SizeType __layer,
                          SizeType __out_item_size) const
    {
        for(SizeType j = 0; j < __out_item_size; ++j)
        {
            _F_get_weighted_sum_forward(__in, __in_size, __out[j], __layer, j);
            __out[j] = __Activation<DataType, DataType>(__out[j]);
        }
    }

    void _F_layer_backward(const DataGroup &__in,
                           SizeType __in_size,
                           DataGroup &__out,
                           SizeType __layer,
                           SizeType __item_num) const
    {
        for(SizeType j = 0; j < __item_num; ++j)
        {
            DataType __sigmoid = __out[j];
            _F_get_weighted_sum_backward(__in, __in_size, __out[j], __layer, j);
            __out[j] = __HResidual<DataType, DataType, DataType>(__out[j], __sigmoid);
        }
    }

    void _F_get_activation(const DataGroup &__in,
                           SizeType __in_size,
                           DataGroup &__out,
                           SizeType __layer,
                           SizeType __out_item_size) const
    {
        _F_realloc(__out, __out_item_size);
        __out.resize(__out_item_size);
        _F_layer_forward(__in, __in_size, __out, __layer, __out_item_size);
    }

    template<typename _InputIterator>
    void _F_set_weight(WeightGroup &__c,
                       _InputIterator __begin,
                       _InputIterator __end,
                       SizeType __s) const
    {
        _F_realloc(__c, __s);
        for(auto it1 = __begin; it1 != __end; ++it1)
        {
            WeightGroupItem group;
            for(auto it2 = (*it1).begin(); it2 != (*it1).end(); ++it2)
            {
                group.push_back(*it2);
            }
            __c.push_back(new WeightGroupItem(group.begin(), group.end()));
        }
    }

    template<typename T>
    void _F_clear(T &__c, SizeType __s = 0) const
    {
        T empty(__s);
        __c.swap(empty);
        __c.clear();
    }
    void _F_realloc_weight_group(WeightGroup &__c, SizeType __s = 0) const
    {
        for(SizeType i = 0; i < __c.size(); ++i)
        {
            WeightGroupItem *__item = __c[i];
            if(__item != nullptr)
            {
                _F_realloc(*__item);
            }
            delete __item;
        }
        _F_clear(__c, __s);
    }
    void _F_realloc_weight_data(WeightData &__c, SizeType __s = 0) const
    {
        for(SizeType i = 0; i < __c.size(); ++i)
        {
            WeightGroup *__group = __c[i];
            if(__group != nullptr)
            {
                _F_realloc(*__group);
            }
            delete __group;
        }
        _F_clear(__c, __s);
    }
    template<typename T>
    void _F_realloc(T &__c, SizeType __s = 0) const
    {
        if
        #if __cplusplus >= 201703L
            constexpr
        #endif
            (std::is_same<T, WeightData>::value)
        {
            _F_realloc_weight_data(__c, __s);
        }
        else if
        #if __cplusplus >= 201703L
            constexpr
        #endif
            (std::is_same<T, WeightGroup>::value)
        {
            _F_realloc_weight_group(__c, __s);
        }
        else
        {
            _F_clear(__c, __s);
        }
    }

    // get the activated value of each layer
    void _F_calculate_activated_value(const DataGroup &__input,
                                      DataGroup *__hide_layer_sigmoid,
                                      DataGroup &__out) const
    {
        _F_get_activation(__input, hide_node_size(),
                          __hide_layer_sigmoid[0], 0, hide_node_size());
        for(SizeType l = 1; l < layer_size(); ++l)
        {
            _F_get_activation(__hide_layer_sigmoid[l - 1], hide_node_size(),
                              __hide_layer_sigmoid[l], l, hide_node_size());
        }
        _F_get_activation(__hide_layer_sigmoid[layer_size() - 1], hide_node_size(),
                          __out, layer_size(), output_size());
    }
    // calculate the error.
    bool _F_calculate_error(const DataGroup &__output, const DataGroup &__out) const
    {
        if(_M_generation > 0) return false;
        for(SizeType n = 0; n < output_size(); ++n)
        {
            ErrorType __e = __Error<DataType, DataType, DataType>(__output[n], __out[n]);
            if(__e > _M_error_bound)
            {
                return false;
            }
        }
        return true;
    }
    // calculate the residual.
    void _F_calculate_residual(const DataGroup &__output,
                               DataGroup *__hide_layer_sigmoid,
                               DataGroup &__out) const
    {
        for(SizeType j = 0; j < output_size(); ++j)
        {
            __out[j] = __OResidual<DataType, DataType, DataType>(__out[j], __output[j]);
        }
        // output -> hide
        _F_layer_backward(__out, output_size(),
                          __hide_layer_sigmoid[layer_size() - 1],
                          layer_size(), hide_node_size());
        // hide -> hide
        for(SizeType l = layer_size() - 1; l > 0; --l)
        {
            _F_layer_backward(__hide_layer_sigmoid[l], hide_node_size(),
                              __hide_layer_sigmoid[l - 1], l, hide_node_size());
        }
    }

    WeightType _F_amplitude(const DataType &__input1,
                            const DataType &__input2,
                            const double __learn_eff) const
    { return __input1 * __input2 * __learn_eff; }

    WeightGroup _F_get_amplitude(const DataGroup &__input1,
                                 const SizeType __input1_size,
                                 const DataGroup &__input2_residual,
                                 const SizeType __input2_size,
                                 const double __learn_eff) const
    {
        WeightGroup __out(__input1_size);
        __out.clear();
        for(auto __it1 = __input1.begin(); __it1 != __input1.end(); ++__it1)
        {
            WeightGroupItem *group = new WeightGroupItem(__input2_size);
            group->clear();
            for(auto __it2 = __input2_residual.begin();
                __it2 != __input2_residual.end(); ++__it2)
            {
                group->push_back(_F_amplitude(*__it1, *__it2, __learn_eff));
            }
            __out.push_back(group);
        }
        return __out;
    }
    void _F_update_weight(WeightGroup &__result,
                          const WeightGroup &__group1,
                          const WeightGroup &__group2,
                          const SizeType __group_size) const
    {
        for(SizeType i = 0; i < __group_size; ++i)
        {
            WeightGroupItem &__item1 = *__group1[i], &__item2 = *__group2[i];
            WeightGroupItem &__result_item = *__result[i];
            for(SizeType j = 0; j < __group_size; ++j)
            {
                __result_item[j] = __item2[j] + __item1[j];
            }
        }
    }
    void _F_update_weight(const DataGroup &__input,
                          const SizeType __input_size,
                          const DataGroup &__residual,
                          const SizeType __residual_size,
                          WeightGroup &__result,
                          const SizeType __result_size) const
    {
        WeightGroup __group = _F_get_amplitude(__input, __input_size,
                                               __residual, __residual_size,
                                               _M_learn_efficency);
        _F_update_weight(__result, __group, __result, __result_size);
    }
    // update weight
    void _F_update_weight(const DataGroup &__input,
                          DataGroup *__hide_layer_error,
                          DataGroup *__hide_layer_sigmoid,
                          const DataGroup &__output_error)
    {
        // input -> hide
        _F_update_weight(__input, hide_node_size(),
                         __hide_layer_error[0], hide_node_size(),
                         *_M_weight[0], hide_node_size());
        // hide -> hide
        for(SizeType i = 0; i < layer_size() - 1; ++i)
        {
            _F_update_weight(__hide_layer_sigmoid[i], hide_node_size(),
                             __hide_layer_error[i + 1], hide_node_size(),
                             *_M_weight[i + 1], hide_node_size());
        }
        // hide -> output
        _F_update_weight(__hide_layer_sigmoid[layer_size() - 1], hide_node_size(),
                         __output_error, output_size(),
                         _M_output_weight, output_size());
    }
    void _F_train(const DataGroup &__input, const DataGroup &__output)
    {
        DataGroup __temp(hide_node_size()), __out;
        DataGroup *__hide_layer_sigmoid = new DataGroup[layer_size()];
        DataGroup *__hide_layer_error = new DataGroup[layer_size()];
        for(SizeType i = 0; _M_generation == 0 || i < _M_generation; ++i)
        {
            _F_calculate_activated_value(__input, __hide_layer_sigmoid, __out);
            for(SizeType n = 0; n < layer_size(); ++n)
            {
                __hide_layer_error[n] = __hide_layer_sigmoid[n];
            }
            bool __meet_end = _F_calculate_error(__output, __out);
            // calculate the residual.
            // [__out] save the residual of the output-layer
            _F_calculate_residual(__output, __hide_layer_error, __out);
            // update weight
            _F_update_weight(__input, __hide_layer_error, __hide_layer_sigmoid, __out);

            if(__meet_end) break;
        }
        for(SizeType i = 0; i < layer_size(); ++i)
        {
            _F_realloc(__hide_layer_sigmoid[i]);
            _F_realloc(__hide_layer_error[i]);
        }
        delete[] __hide_layer_sigmoid;
        delete[] __hide_layer_error;
    }
};

}

#endif // NEURALNETWORK_H
