/*
Dwarf Therapist
Copyright (c) 2009 Trey Stout (chmod)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef ROLESTATS_H
#define ROLESTATS_H

#include <QObject>
#include <QVector>
#include <math.h>
#include "ecdf.h"
#include "gamedatareader.h"
#include "truncatingfilelogger.h"

class RoleStats{

public:
    typedef enum{
        TT_UNKNOWN, //indicates that the transformation method needs to be determined with various tests on the data
        TT_ECDF_RANK_TRY, //attempt to use ecdf_rank_upper, but if it fails switch to ecdf_rank_all
        TT_ECDF_RANK_UPPER, //ecdf_rank on upper, factor on lower
        TT_ECDF_RANK_ALL, //ecdf rank on everything
        TT_MULTI_UPPER, //multiple tranformations on upper, factor on lower
        TT_MULTI_ALL, //multiple transformations on all
        TT_MIN_MAX_UPPER, //min-max on upper, factor on lower
        TT_MIN_MAX_ALL //scale from 0-50 and 50-100
    } TRANSFORM_TYPE;

    RoleStats(const QVector<double> &unsorted);
    RoleStats(const QVector<double> &unsorted, const TRANSFORM_TYPE t);
    virtual ~RoleStats(){
//        m_raws = 0;
//        m_upper = 0;
    }

    struct transform_stats{
        double average;
        double median;
        double min;
        double max;
    };

    double get_rating(double val);
    void set_list(const QVector<double> &unsorted);

private:
    QSharedPointer<ECDF> m_raws; //primary ecdf based on the initial vector
    QSharedPointer<ECDF> m_upper; //special ecdf for values over the median. used if min = median
    double m_ecdf_median; //median of m_sorted.favg values
    double m_raw_median;
    double m_sum_over_median; //sum of all the values of m_sorted > median
    double m_sum_upper; //sum ((fplus(x) + fminus(x))/4.0)+0.5 where x is a value in m_upper
    double m_factor;
    double m_upper_minmax_diff;
    double m_upper_raw_min; //the raw value associated with the first upper value
    double m_transform_one_percent;
    TRANSFORM_TYPE m_transform_type;
    void init_list();

    double find_median(QVector<double> v);
    double range_transform(double val, double min, double mid, double max);

    QList<transform_stats> m_transformations;
    bool load_transformations(QVector<double> list);
    transform_stats load_list_stats(const QVector<double> list, bool save = true);
    void calculate_factor_value(int upper_start_idx);

    double get_transformations_rating(double val);
    bool transform_valid(transform_stats ts, bool mid_is_avg);

    void configure_transformations();
};

#endif // ROLESTATS_H
