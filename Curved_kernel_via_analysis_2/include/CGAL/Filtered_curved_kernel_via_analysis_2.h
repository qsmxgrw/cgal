// TODO: Add licence
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Eric Berberich <eric@mpi-inf.mpg.de>
//                 Pavel Emeliyanenko <asm@mpi-sb.mpg.de>
//
// ============================================================================

#ifndef CGAL_FILTERED_CURVED_KERNEL_VIA_ANALYSIS_2_H
#define CGAL_FILTERED_CURVED_KERNEL_VIA_ANALYSIS_2_H

/*! \file Filtered_curved_kernel_via_analysis_2.h
 *  \brief defines class \c Filtered_curved_kernel_via_analysis_2
 *  
 * Defines points and arcs supported by curves that can be analyzed
 * and where some operations are filtered.
 */

#include <CGAL/basic.h>
#include <CGAL/Curved_kernel_via_analysis_2.h>
#include <CGAL/Curved_kernel_via_analysis_2/Curved_kernel_via_analysis_2_functors.h>

#include <CGAL/Bbox_2.h>

CGAL_BEGIN_NAMESPACE

#define CKvA_DEBUG_PRINT_CERR 1

#ifndef CKvA_CERR
//#define CKvA_DEBUG_PRINT_CERR
#ifdef CKvA_DEBUG_PRINT_CERR
#define CKvA_CERR(x) std::cout << x
#else
#define CKvA_CERR(x) static_cast<void>(0)
#endif
#endif

namespace CGALi {

namespace Filtered_curved_kernel_via_analysis_2_Functors {

template < class CurvedKernelViaAnalysis_2 >
class Compare_xy_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Compare_xy_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Compare_xy_2< 
    Curved_kernel_via_analysis_2 > Base;

    //! the result type
    typedef CGAL::Comparison_result result_type;
    typedef Arity_tag<2>            Arity;
    
    //! standard constructor
    Compare_xy_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }

    /*!
     * Compares two points lexigoraphically: by x, then by y.
     * \param p1 The first point.
     * \param p2 The second point.
     * \return LARGER if x(p1) > x(p2), or if x(p1) = x(p2) and y(p1) > y(p2);
     *         SMALLER if x(p1) \< x(p2), or if x(p1) = x(p2) and 
     *                   y(p1) \< y(p2);
     *         EQUAL if the two points are equal.
     */
    result_type operator()(const Point_2& p1, const Point_2& p2,
                           bool equal_x = false) const {

        CKvA_CERR("\nfilteredcompare_xy_; p1: " << p1 << "; p2: " <<
             p2 << "\n");
        
        Base base_compare_xy(this->_ckva());

        result_type res = base_compare_xy(p1, p2, equal_x);

        CKvA_CERR("result: " << res << "\n");
        
        return res;
    }
};

template < class CurvedKernelViaAnalysis_2 >
class Compare_y_near_boundary_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Compare_y_near_boundary_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Compare_y_near_boundary_2< 
    Curved_kernel_via_analysis_2 > Base;
    
    //! the result type
    typedef CGAL::Comparison_result result_type;
    typedef Arity_tag<3>            Arity;
    
    //! standard constructor
    Compare_y_near_boundary_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }

    /*! Compare the y-coordinates of 2 lines at their ends near the boundary
     * of the parameter space at x = +/- oo.
     * \param xcv1 the first arc.
     * \param xcv2 the second arc.
     * \param ce the line end indicator.
     * \return the second comparison result.
     * \pre the ce ends of the lines xcv1 and xcv2 lie either on the left
     * boundary or on the right boundary of the parameter space.
     */
    result_type operator()(const Arc_2& cv1, const Arc_2& cv2,
                           CGAL::Arr_curve_end ce) const {
        
        CKvA_CERR("\nfilteredcompare_y_near_boundary; cv1: " << cv1 
                  << "; cv2: " << cv2 << "; end: " << ce << "\n");

        CGAL::Arr_parameter_space loc1 = cv1.location(ce);
        CGAL_precondition(cv1.is_on_left_right(loc1) &&
                          loc1 == cv2.location(ce));
        // comparing ids is the same as calling is_identical() ??
        if (cv1.id() == cv2.id()) {
            return CGAL::EQUAL;
        } 
        
        typedef typename Arc_2::Curve_kernel_2 Curve_kernel_2;

        typedef typename Curve_kernel_2::X_coordinate_1 X_coordinate_1;

        CGAL::Object obj1, obj2;
        X_coordinate_1 asym_info1, asym_info2;
        CGAL::Arr_parameter_space ps1, ps2;
        
        obj1 = cv1.curve().asymptotic_value_of_arc(
                cv1.location(ce), cv1.arcno()
        );
        obj2 = cv2.curve().asymptotic_value_of_arc(
                cv2.location(ce), cv2.arcno()
        );
        
        CGAL::Comparison_result filter_res = CGAL::EQUAL;
        
        if (CGAL::assign(ps1, obj1)) {
            if (CGAL::assign(ps2, obj2)) {
                if (ps1 == ps2) {
                    filter_res = CGAL::EQUAL;
                } else {
                    filter_res = (ps2 == CGAL::ARR_TOP_BOUNDARY ?
                                  CGAL::SMALLER : CGAL::LARGER);
                }
            } else {
                CGAL_assertion(CGAL::assign(asym_info2, obj2));
                filter_res = (ps1 == CGAL::ARR_TOP_BOUNDARY ?
                              CGAL::LARGER : CGAL::SMALLER);
            }
        } else {
            CGAL_assertion_code(bool check = )
                CGAL::assign(asym_info1, obj1);
            CGAL_assertion(check);
            if (CGAL::assign(ps2, obj2)) {
                filter_res = (ps2 == CGAL::ARR_TOP_BOUNDARY ?
                              CGAL::SMALLER : CGAL::LARGER);
            } else {
                CGAL_assertion_code(bool check = )
                    CGAL::assign(asym_info2, obj2);
                CGAL_assertion(check);
                filter_res = this->_ckva()->kernel().compare_x_2_object()(
                        asym_info1, asym_info2
                );
            }
        }
        
        if(filter_res != CGAL::EQUAL) {
            std::cout << "filtered!" << std::endl;
            return filter_res;
        }
        std::cout << "filter failed" << std::endl;

        Base base_compare_y_near_boundary(this->_ckva());

        result_type res = base_compare_y_near_boundary(cv1, cv2, ce);
        
        CKvA_CERR("result: " << res << "\n");
        return res;
    }
};

template < class CurvedKernelViaAnalysis_2 >
class Compare_y_at_x_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Compare_y_at_x_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Compare_y_at_x_2< 
    Curved_kernel_via_analysis_2 > Base;
    
    //! the result type
    typedef CGAL::Comparison_result result_type;
    typedef Arity_tag<2>            Arity;
    
    //! standard constructor
    Compare_y_at_x_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }

    /*!
     * Return the location of the given point with respect to the input curve.
     * \param cv The curve.
     * \param p The point.
     * \pre p is in the x-range of cv.
     * \return SMALLER if y(p) \< cv(x(p)), i.e. the point is below the curve;
     *         LARGER if y(p) > cv(x(p)), i.e. the point is above the curve;
     *         EQUAL if p lies on the curve.
     */
    result_type operator()(const Point_2& p, const Arc_2& cv) const {
     
        CKvA_CERR("\nfilteredcompare_y_at_x; p: " << p << ";\n cv:" << cv << "\n");
        
        Base base_compare_y_at_x(this->_ckva());

        result_type res = base_compare_y_at_x(p, cv);
        
        CKvA_CERR("result: " << res << "\n");
        return res;
    }
};

template < class CurvedKernelViaAnalysis_2 >
class Compare_y_at_x_left_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Compare_y_at_x_left_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Compare_y_at_x_left_2< 
    Curved_kernel_via_analysis_2 > Base;

    //! the result type
    typedef CGAL::Comparison_result result_type;
    typedef Arity_tag<3>            Arity;
    
    //! standard constructor
    Compare_y_at_x_left_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }
    
    /*!
     * Compares the y value of two x-monotone curves immediately to the left
     * of their intersection point. If one of the curves is vertical
     * (emanating downward from p), it's always considered to be below the
     * other curve.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param p The intersection point.
     * \pre The point p lies on both curves, and both of them must be also be
     *      defined (lexicographically) to its left.
     * \return The relative position of cv1 with respect to cv2 immdiately to
     *         the left of p: SMALLER, LARGER or EQUAL.
     */
    result_type operator() (const Arc_2& cv1, const Arc_2& cv2,
                            const Point_2& p) const {

        CKvA_CERR("\ncompare_y_at_x_left(cv2); cv1: " << cv1 << "; cv2: " <<
            cv2 << "; p: " << p << "\n");

        
        Base base_compare_y_at_x_left(this->_ckva());

        result_type res = base_compare_y_at_x_left(cv1, cv2, p);
        
        CKvA_CERR("result: " << res << "\n");
        return res;
    }
};

template < class CurvedKernelViaAnalysis_2 >
class Compare_y_at_x_right_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Compare_y_at_x_right_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Compare_y_at_x_right_2< 
    Curved_kernel_via_analysis_2 > Base;

    //! the result type
    typedef CGAL::Comparison_result result_type;
    typedef Arity_tag<3>            Arity;
    
    //! standard constructor
    Compare_y_at_x_right_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }
    
    /*!
     * Compares the y value of two x-monotone curves immediately to the right
     * of their intersection point. If one of the curves is vertical
     * (emanating downward from p), it's always considered to be below the
     * other curve.
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param p The intersection point.
     * \pre The point p lies on both curves, and both of them must be also be
     *      defined (lexicographically) to its right.
     * \return The relative position of cv1 with respect to cv2 immdiately to
     *         the right of p: SMALLER, LARGER or EQUAL.
     */
    result_type operator() (const Arc_2& cv1, const Arc_2& cv2,
                            const Point_2& p) const {

        CKvA_CERR("\ncompare_y_at_x_right(cv2); cv1: " << cv1 << "; cv2: " <<
            cv2 << "; p: " << p << "\n");

        
        Base base_compare_y_at_x_right(this->_ckva());

        result_type res = base_compare_y_at_x_right(cv1, cv2, p);
        
        CKvA_CERR("result: " << res << "\n");
        return res;
    }
};

//!\brief Tests two objects, whether two arcs can have an intersection
template < class CurvedKernel_2 >
class May_have_intersection_2 {

    typedef typename CurvedKernel_2::Point_2 Point_2;
    typedef typename CurvedKernel_2::Arc_2 Arc_2;

private:

    typedef typename Arc_2::Curve_kernel_2 Curve_kernel_2;
    
    typedef typename Curve_kernel_2::Curve_analysis_2 Curve_analysis_2;
    typedef typename Curve_kernel_2::X_coordinate_1 X_coordinate_1;
    typedef typename Curve_kernel_2::Xy_coordinate_2 Xy_coordinate_2;
    typedef typename Curve_kernel_2::X_real_traits_1 X_real_traits_1;
    typedef typename Curve_kernel_2::Y_real_traits_1 Y_real_traits_1;
    
    typename X_real_traits_1::Lower_boundary x_low;
    typename X_real_traits_1::Upper_boundary x_high;
    typename X_real_traits_1::Refine x_refine;
    
    typename Y_real_traits_1::Lower_boundary y_low;
    typename Y_real_traits_1::Upper_boundary y_high;
    typename Y_real_traits_1::Refine y_refine;
    
    typedef typename X_coordinate_1::Rational Boundary;
    
    static double& threshold() {
        return Curve_kernel_2::threshold();
    }

public:
    typedef bool result_type;
    typedef Arity_tag<2> Arity;
    
    //! standard constructor
    May_have_intersection_2(CurvedKernel_2 *kernel) :
        _m_curved_kernel(kernel) {
        CGAL_assertion(kernel != NULL);
    }
    
    /*!\brief
     * Checks whether \c cv1 and \c cv2 can have an intersection. If
     * not it certainly returns false, if possible, it return true.
     */
    template < class Arc_2_ >
    bool operator()(const Arc_2_& cv1, const Arc_2_& cv2) const {
        
        std::list< CGAL::Bbox_2 > boxes1, boxes2;
        
        construct_covering_approximation(cv1, std::back_inserter(boxes1));
        construct_covering_approximation(cv2, std::back_inserter(boxes2));

        if (!boxes1.empty() && !boxes2.empty()) {
            // TODO better strategy than quadratic pair of for-loops
            for (typename std::list< CGAL::Bbox_2 >::const_iterator bit1 =
                     boxes1.begin(); bit1 != boxes1.end(); bit1++) {
                for (typename std::list< CGAL::Bbox_2 >::const_iterator bit2 =
                         boxes2.begin(); bit2 != boxes2.end(); bit2++) {
                    if (CGAL::do_overlap(*bit1, *bit2)) {
                        return true;
                    }
                } 
            }
        } 
        
        return false;
    }

private:

    std::pair<double,double> y_interval_for_arc_end(const Arc_2& arc,
                                                    CGAL::Arr_curve_end end) 
    const {
        
        double min, max;

        switch(arc.location(end)) {
            
        case(CGAL::ARR_LEFT_BOUNDARY): 
        case(CGAL::ARR_RIGHT_BOUNDARY): {
            
            CGAL::Object obj = arc.curve().asymptotic_value_of_arc(
                    arc.location(end), arc.arcno()
            );
            
            CGAL::Arr_parameter_space ps;
            X_coordinate_1 asym_info;
            
            if (CGAL::assign(ps, obj)) {
                if (ps == CGAL::ARR_BOTTOM_BOUNDARY) {
                    min = max = -numeric_limits<double>::infinity();
                } else {
                    CGAL_assertion(ps == CGAL::ARR_TOP_BOUNDARY);
                    min = max = numeric_limits<double>::infinity();
                }
            } else {
                CGAL_assertion_code(bool check =) 
                    CGAL::assign(asym_info, obj);
                CGAL_assertion(check);
                while(x_high(asym_info) - 
                      x_low(asym_info) > threshold()) {
                    x_refine(asym_info);
                }
                min = CGAL::to_interval(x_low(asym_info)).first;
                max = CGAL::to_interval(x_high(asym_info)).second;
            }
        }
        case(CGAL::ARR_TOP_BOUNDARY): {
            min = max = numeric_limits<double>::infinity();
            break;
        }
        case(CGAL::ARR_BOTTOM_BOUNDARY): {
            min = max = -numeric_limits<double>::infinity();
            break;
        }
        case(CGAL::ARR_INTERIOR): {
            CGAL::Bbox_2 point_bbox 
                = arc.curve_end(end).xy().approximation_box_2(threshold());
            min = point_bbox.xmin();
            max = point_bbox.xmax();
            break;
        }
        }

        return std::make_pair(min,max);

    }

    void update_y(double& y_min, double& y_max, 
                  std::pair<double,double> y_iv) const {

        if(y_iv.first < y_min) {
            y_min = y_iv.first;
        }
        if(y_iv.second > y_max) {
            y_max = y_iv.second;
        }

    }



public:

    /*!\brief
     * Constructs for a given \c arc its covering approximation.
     */
    template < class OutputIterator >
    OutputIterator construct_covering_approximation(
            const Arc_2& arc, OutputIterator oi
    ) const {
        CKvA_CERR("\nconstruct_covering_approximation; arc: " << arc 
             << ";\n cv:" << arc << "\n");

        // TODO use cache for this construction

        double x_min, x_max, y_min, y_max;
        boost::optional<X_coordinate_1> left, right;


        if( arc.location(CGAL::ARR_MIN_END) == CGAL::ARR_LEFT_BOUNDARY ) {
            x_min = -numeric_limits<double>::infinity();
        } else {
            left = arc.curve_end_x(CGAL::ARR_MIN_END);
            while(x_high(left.get())-x_low(left.get()) > threshold()) {

                x_refine(left.get());

            }
            x_min = CGAL::to_interval(x_low(left.get())).first;
        }

        if( arc.location(CGAL::ARR_MAX_END) == CGAL::ARR_RIGHT_BOUNDARY ) {
            x_max = numeric_limits<double>::infinity();
        } else {
            right = arc.curve_end_x(CGAL::ARR_MAX_END);
            while(x_high(right.get())-x_low(right.get()) > threshold()) {
                x_refine(right.get());
            }
            x_max = CGAL::to_interval(x_high(right.get())).second;
        }

        typename Curve_analysis_2::Internal_curve_2 curve =
             arc.curve()._internal_curve();
        int arcno = arc.arcno();

        int y_crit_begin = 0, y_crit_end = curve.num_y_critical_coordinates();

        if(left) {
            
            while( y_crit_begin < y_crit_end && 
                   curve.y_critical_coordinate(y_crit_begin) <= left.get()) {
                y_crit_begin++;
            }
        }

        if(right) {
            
            while( y_crit_end > y_crit_begin &&
                   curve.y_critical_coordinate(y_crit_end-1) >= right.get() ) {
                y_crit_end--;
            }
        }

        y_min = numeric_limits<double>::infinity();
        y_max = -numeric_limits<double>::infinity();

        update_y(y_min,y_max,y_interval_for_arc_end(arc,CGAL::ARR_MIN_END));
        update_y(y_min,y_max,y_interval_for_arc_end(arc,CGAL::ARR_MAX_END));

        for(int i = y_crit_begin; i < y_crit_end; i++) {
            
            Xy_coordinate_2 curr_xy(curve.y_critical_coordinate(i),
                                    arc.curve(),
                                    arcno);
            CGAL::Bbox_2 point_bbox = curr_xy.approximation_box_2(threshold());
            std::pair<double,double> y_iv = std::make_pair(point_bbox.ymin(),
                                                           point_bbox.ymax());
            update_y(y_min,y_max,y_iv);

        }

        CGAL::Bbox_2 bbox(x_min, y_min, x_max, y_max);
        
        CKvA_CERR("\nres: " << bbox << "\n");
        
        *oi++ = bbox;
        return oi;
    }

private:
    //! pointer to \c CurvedKernel_2 ?
    CurvedKernel_2 *_m_curved_kernel;
};


//! checks wether and how two arcs are intersection - with first filtering
template < class CurvedKernelViaAnalysis_2 >
class Intersect_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Intersect_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Intersect_2< 
    Curved_kernel_via_analysis_2 > Base;

    //! standard constructor
    Intersect_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }
    
    /*!
     * Find all intersections of the two given curves and insert them to the 
     * output iterator. If two arcs intersect only once, only a single will be
     * placed to the iterator. Type of output iterator is \c CGAL::Object 
     * containing either an \c Arc_2 object (overlap) or a \c Point_2 object
     * with multiplicity (point-wise intersections)
     * \param cv1 The first curve.
     * \param cv2 The second curve.
     * \param oi The output iterator.
     * \return The past-the-end iterator.
     */
    template < class OutputIterator >
    OutputIterator operator()(const Arc_2& cv1, const Arc_2& cv2,
                              OutputIterator oi) const {

        CKvA_CERR("\nfiltered_intersect; cv1: " << cv1 
             << ";\n cv2:" << cv2 << "");

        if (!this->_ckva()->may_have_intersection_2_object()(cv1, cv2)) {
            // return no one
            CKvA_CERR("\nfilter: sucessfull\n");
            return oi;
        }

        // else 
        CKvA_CERR("\nfilter: failed\n");

        // and call usual intersection
        std::list< CGAL::Object > tmp;
        Base base_intersection(this->_ckva());
        base_intersection(cv1, cv2, std::back_inserter(tmp));
        for (std::list< CGAL::Object >::const_iterator it = tmp.begin();
             it != tmp.end(); it++) {
            *oi++ = *it;
        }
        return oi;
    }
};

template < class CurvedKernelViaAnalysis_2 >
class Is_on_2 : 
        public Curved_kernel_via_analysis_2_Functors::
            Is_on_2< CurvedKernelViaAnalysis_2 > {
    
public:
    //! this instance template parameter
    typedef CurvedKernelViaAnalysis_2 Curved_kernel_via_analysis_2;

    //! type of curve
    typedef typename Curved_kernel_via_analysis_2::Curve_2 Curve_2;

    //! type of point
    typedef typename Curved_kernel_via_analysis_2::Point_2 Point_2;

    //! type of arc
    typedef typename Curved_kernel_via_analysis_2::Arc_2 Arc_2;

    //! the bae type
    typedef typename 
    Curved_kernel_via_analysis_2_Functors::Is_on_2< 
    Curved_kernel_via_analysis_2 > Base;

    //! the result type
    typedef bool result_type;
    typedef Arity_tag<2> Arity;
    
    //! standard constructor
    Is_on_2(Curved_kernel_via_analysis_2 *kernel) :
        Base(kernel) {
    }
    
    /*!
     * Checks whether \c p lies on \c c 
     * \param p1 The point to test
     * \param p2 The curve
     * \return (true) if the \c p lies on \c c
     */
    result_type operator()(const Point_2& p, const Curve_2& c) const {
        
        CKvA_CERR("\nfiltered_is_on; p: " << p << ";\n c:" << c << "");
        
        Base base_is_on(this->_ckva());
        
        result_type res = base_is_on(p, c);
        
        CKvA_CERR("result: " << res << "\n");
        
        return res;
    }
};

} // namespace Filtered_curved_kernel_via_analysis_2_Functors 

} // namespace CGALi

/*!\brief
 * Filtered curved kernel, i.e., intersection predicate is filted by first
 * computing a covering approximation. Only if these overlap for two arcs
 * the exact intersection predicate is called.
 */
template < class CurveKernel_2 >

class Filtered_curved_kernel_via_analysis_2 :
     public CGALi::Curved_kernel_via_analysis_2_base < 
         Filtered_curved_kernel_via_analysis_2< CurveKernel_2 >, 
         CurveKernel_2 
     >,
     public CGALi::Curved_kernel_via_analysis_2_functors < 
            Filtered_curved_kernel_via_analysis_2< CurveKernel_2 >,
            typename CurveKernel_2::Curve_analysis_2,
            CGALi::Point_2 < 
                Filtered_curved_kernel_via_analysis_2< CurveKernel_2 > 
            >,
            CGALi::Arc_2 < 
                Filtered_curved_kernel_via_analysis_2< CurveKernel_2 > 
            > > {


public:
    //! \name public typedefs
    //!@{
    
    //! this instance's template argument
    typedef CurveKernel_2 Curve_kernel_2;

    //! myself
    typedef Filtered_curved_kernel_via_analysis_2< Curve_kernel_2 > Self;
    
    //!@}

    //!\name embedded types  for \c Arrangement_2 package
    //!@{

    //! type of curve_2
    typedef typename Curve_kernel_2::Curve_analysis_2 Curve_2;
        
    //! type of a point on generic curve
    typedef CGALi::Point_2< Self > Point_2; 

    //! type of an arc on generic curve
    typedef CGALi::Arc_2< Self > Arc_2; 

    //! type of weakly x-monotone arc for \c ArrangementTraits_2
    typedef Arc_2 X_monotone_curve_2;

    //!@}

    //!\name Additional functors
    //!{
    
// declares curved kernel functors, for each functor defines a member function
// returning an instance of this functor
#define CGAL_CKvA_2_functor_pred(Y, Z) \
    typedef CGALi::Curved_kernel_via_analysis_2_Functors::Y<Self> Y; \
    Y Z() const { return Y((Filtered_curved_kernel_via_analysis_2 *)this); }

#define CGAL_CKvA_2_functor_cons(Y, Z) CGAL_CKvA_2_functor_pred(Y, Z)
    
public:

    CGAL_CKvA_2_functor_cons(Construct_point_2, 
                             construct_point_2_object);
    
    CGAL_CKvA_2_functor_cons(Construct_point_on_arc_2, 
                             construct_point_on_arc_2_object);
    
    CGAL_CKvA_2_functor_cons(Construct_arc_2, 
                             construct_arc_2_object);
    
// declares curved kernel functors, for each functor defines a member function
// returning an instance of this functor
#define CGAL_FILTERED_CKvA_2_functor_pred(Y, Z) \
    typedef CGALi::Filtered_curved_kernel_via_analysis_2_Functors::Y<Self> Y; \
    Y Z() const { return Y((Filtered_curved_kernel_via_analysis_2 *)this); }

#define CGAL_FILTERED_CKvA_2_functor_cons(Y, Z) \
    CGAL_FILTERED_CKvA_2_functor_pred(Y, Z)
    
    CGAL_FILTERED_CKvA_2_functor_pred(Compare_xy_2, compare_xy_2_object);
    
    CGAL_FILTERED_CKvA_2_functor_pred(Compare_y_near_boundary_2, 
                                      compare_y_near_boundary_2_object);

    CGAL_FILTERED_CKvA_2_functor_pred(Compare_y_at_x_2, 
                                      compare_y_at_x_2_object);
    CGAL_FILTERED_CKvA_2_functor_pred(Compare_y_at_x_left_2, 
                                      compare_y_at_x_left_2_object);
    CGAL_FILTERED_CKvA_2_functor_pred(Compare_y_at_x_right_2, 
                                      compare_y_at_x_right_2_object);

    CGAL_FILTERED_CKvA_2_functor_pred(Is_on_2, is_on_2_object);

    CGAL_FILTERED_CKvA_2_functor_pred(
            May_have_intersection_2, may_have_intersection_2_object
    );
    
    CGAL_FILTERED_CKvA_2_functor_cons(Intersect_2, intersect_2_object);
    
#undef CGAL_FILTERED_CKvA_2_functor_pred
#undef CGAL_FILTERED_CKvA_2_functor_cons

    //!@}
    
protected:
    //! protected internal types
    //!@{
    
    //! class collecting basic types
    typedef CGALi::Curved_kernel_via_analysis_2_base < Self, CurveKernel_2 >
    Base_kernel;

    //! class collecting basic types
    typedef CGALi::Curved_kernel_via_analysis_2_functors < 
            Self, Curve_2, Point_2, Arc_2
    >  
    Base_functors;

    //!@}
    
public:
    //! \name Constructors
    //!@{

    //! default constructor
    Filtered_curved_kernel_via_analysis_2() :
        Base_kernel() {
    }
    
    //! construct using specific \c Curve_kernel_2 instance (for controlling)
    Filtered_curved_kernel_via_analysis_2(const Curve_kernel_2& kernel) :
        Base_kernel(kernel) {
    }
    
    //!@}

}; // class Curved_kernel_via_analysis_2

CGAL_END_NAMESPACE

#endif // CGAL_CURVED_KERNEL_VIA_ANALYSIS_2_H
