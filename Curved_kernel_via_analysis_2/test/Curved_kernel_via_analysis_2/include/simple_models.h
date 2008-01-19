// TODO: Add licence
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL:$
// $Id: $
// 
//
// Author(s)     : Pavel Emeliyanenko <asm@mpi-sb.mpg.de>
//
// ============================================================================

/*! \file simple_models.h
 *  \brief defines dummy implementations satisfying Curve_kernel_2
 *  concept requirenments 
 */

#ifndef CGAL_CKVA_2_SIMPLE_MODELS_H
#define CGAL_CKVA_2_SIMPLE_MODELS_H

#include <CGAL/basic.h>
#include <CGAL/Polynomial.h>

CGAL_BEGIN_NAMESPACE

namespace CGALi {

//////////////////////////////////////////////////////////////////////////////

struct X_coordinate_rep {

    int bla_;

    // DefaultConstructible
    X_coordinate_rep() :
        bla_(0) {
    }
};

struct X_coordinate_1 :
     public ::CGAL::Handle_with_policy< X_coordinate_rep > {

    typedef X_coordinate_rep         Rep;
    typedef ::CGAL::Handle_with_policy< Rep > Base;
    
    // default constructor
    X_coordinate_1() : Base(Rep()) {
    }

    bool operator ==(const X_coordinate_1&) const
    { return false; }
};

struct Curve_2_model_rep {
    int i_;

    typedef CGAL::Polynomial< CGAL::Polynomial < int > > Poly_d;
    Poly_d f_;

    // DefaultConstructible
    Curve_2_model_rep() :
        i_(0) {
    }
    
    Curve_2_model_rep(int i) :
        i_(i) {
    }
};

struct Curve_2_model :
    public ::CGAL::Handle_with_policy< Curve_2_model_rep > {

    typedef Curve_2_model_rep         Rep;
    typedef ::CGAL::Handle_with_policy< Rep > Base;
    
    typedef X_coordinate_1 X_coordinate;
    typedef X_coordinate_1 Y_coordinate;
    typedef double Boundary;

    typedef int Coefficient;
    
    //typedef SoX::Event1_info< X_coordinate > Event1_info;

    typedef CGAL::Polynomial< CGAL::Polynomial < int > > Poly_d;

    typedef CGAL::Handle_id_less_than< Curve_2_model > Less_than;
    
    // for total_degree (find smaller curve if two are available)
    Poly_d f() const {
        return ptr()->f_;
    }

    int num_events() const {
        return 0;
    }

    void x_to_index(X_coordinate x, int& idx, bool& event) const {
        return;
    }

    Boundary boundary_value_in_interval(int i) {
        return Boundary(0);
    }

    Y_coordinate y_at(Boundary r, int arcno){ 
        return Y_coordinate();
    }

    int arcs_over_interval(int id) const {
        // this values are needed for the Event1_info.C test
        if ((id % 2) == 0) {
            return 10;
        } else {
            return 11;
        }
    }

    template < class OutputIterator >
    static bool decompose(Curve_2_model f, Curve_2_model g,
                   OutputIterator parts_of_f,
                   OutputIterator parts_of_g) {
        return true;    
    }

    bool operator== (const Curve_2_model& c) {
        return id() == c.id();
    }
};

std::ostream& operator<< (std::ostream& os, Curve_2_model c) {
    return os;
}

std::istream& operator>> (std::istream& is, Curve_2_model& c) {
    return is;
}

///////////////////////////////////////////////////////////////////////////////// Curve_pair_2

template < class Curve_ >
struct Curve_pair_2_model;

template < class Curve_ >
struct Curve_pair_2_model_rep {

    typedef Curve_ Curve;
    
    typedef Curve Algebraic_curve_2;

    //typedef SoX::Event2_slice< Curve_pair_2< Curve > > Event2_slice;
    
    Curve c1_;
    Curve c2_;
    
    // DefaultConstructible
    Curve_pair_2_model_rep() :
        c1_(), c2_() {
    }

    Curve_pair_2_model_rep(Curve c1, Curve c2) :
        c1_(c1), c2_(c2) {
    }
    
    std::vector< int > slices_;
};

template < class Curve_ >
struct Curve_pair_2_model : 
    public ::CGAL::Handle_with_policy< Curve_pair_2_model_rep< Curve_ > > {
    
    typedef Curve_ Curve;

    typedef Curve Algebraic_curve_2;

    typedef Curve_pair_2_model_rep< Curve > Rep;
    typedef ::CGAL::Handle_with_policy< Rep >        Base;

    //typedef SoX::Event2_slice< Curve_pair_2< Curve > > Event2_slice;

    // DefaultConstructible
    Curve_pair_2_model() :
        Base(Rep()) {
    };

    // Assignable
    
    // Constructable from two curves
    Curve_pair_2_model(Curve c1, Curve c2) :
        Base(Rep(c1, c2)) {
    }

    Curve curve1() const {
        return this->ptr()->c1_;
    }

    Curve curve2() const {
        return this->ptr()->c2_;
    }

    int num_events() const {
        return 0;
    }

    int event_x(int i) const {
        return -1;
    }

    void x_to_index(typename Algebraic_curve_2::X_coordinate x, 
                    int& idx, bool& event) const {
        return;
    }
};

/////////////////////////////////////////////////////////////////////////////

template < class AlgebraicCurveKernel_2>
class Xy_coordinate_2;

template < class AlgebraicCurveKernel_2 >
class Xy_coordinate_2_rep {

public:
    // this first template argument
    typedef AlgebraicCurveKernel_2 Algebraic_curve_kernel_2;

    // myself
    typedef Xy_coordinate_2_rep<Algebraic_curve_kernel_2> Self;

    typedef typename Algebraic_curve_kernel_2::Curve_pair_2 Curve_pair_2;

    typedef typename Curve_pair_2::Algebraic_curve_2 Curve_2; 
    
    typedef typename Curve_2::X_coordinate X_coordinate_1; 

    // constructors
public:
    // default constructor ()
    Xy_coordinate_2_rep()
    {   }
    
    // data
    // x-coordinate
    X_coordinate_1 _m_x;
    
    // supporting curve
    mutable Curve_2 _m_curve;
    
    // arc number on curve
    mutable int _m_arcno;

    // befriending the handle
    friend class Xy_coordinate_2<Algebraic_curve_kernel_2>;
};

template <class AlgebraicCurveKernel_2>
class Xy_coordinate_2 : 
    public
       ::CGAL::Handle_with_policy<Xy_coordinate_2_rep<AlgebraicCurveKernel_2> >
{
public:
    //! \name public typedefs
    //!@{
    
    //! this instance's first template parameter
    typedef AlgebraicCurveKernel_2 Algebraic_curve_kernel_2;
    
    //! this instance's second template parameter
    typedef Xy_coordinate_2_rep<AlgebraicCurveKernel_2> Rep;
    
    //! this instance itself
    typedef Xy_coordinate_2<Algebraic_curve_kernel_2> Self;
        
    //! type of a curve pair
    typedef typename Algebraic_curve_kernel_2::Curve_pair_2 Curve_pair_2;

    //! type of an algabraic curve
    typedef typename Curve_pair_2::Algebraic_curve_2 Curve_2; 

    //! type of X_coordinate
    typedef typename Curve_2::X_coordinate X_coordinate_1;

    //! the handle superclass
    typedef ::CGAL::Handle_with_policy<Rep> Base;

    typedef typename Algebraic_curve_kernel_2::X_real_traits_1 X_real_traits_1;

    //! type for approximation boundaries
    typedef typename X_real_traits_1::Boundary Boundary;

    //! type for boundary intervals
    typedef std::pair<Boundary, Boundary> Boundary_interval;

    //!@}
public:
    //!\name Constructors
    //!@{

    Xy_coordinate_2() :
        Base(Rep()) { 
    }

    Xy_coordinate_2(const Self& p) :
        Base(static_cast<const Base&>(p)) {  
    }

    Xy_coordinate_2(const X_coordinate_1&, const Curve_2&, int) :
            Base(Rep()) {
    }
    
    Xy_coordinate_2(Rep rep) :
        Base(rep) {  
    }
   
public:
    
    X_coordinate_1 x() const {
        return this->ptr()->_m_x;
    }

    X_coordinate_1 y() const {
        return this->ptr()->_m_x;
    }
    
    Curve_2 curve() const {
        return this->ptr()->_m_curve; 
    }
    
    int arcno() const {
        return -1; 
    }

    //!@}
public:
    //!\name comparison predicates
    //!@{

    CGAL::Comparison_result compare_x(const Self& q) const {
        return CGAL::ZERO;
    }

    CGAL::Comparison_result compare_xy(const Self& q,
            bool equal_x = false) const {
        return CGAL::ZERO;
    }
    
    //! equality
    bool operator == (const Self& q) const {return false;}
    
    //! inequality
    bool operator != (const Self& q) const {return false;}

    //! less than in (x,y) lexicographic order
    bool operator <  (const Self& q) const {return false;}

    //! less-equal in (x,y) lexicographic order
    bool operator <= (const Self& q) const {return false;}

    //! greater than in (x,y) lexicographic order
    bool operator >  (const Self& q) const {return false;}

    //! greater-equal in (x,y) lexicographic order
    bool operator >= (const Self& q) const {return false;}
    
public:

    bool is_x_zero() const {
        return false;
    }

    bool is_y_zero() const {
        return false;
    }
    
    std::pair<double, double> to_double() const {
        return std::make_pair(0.0, 0.0);
    }

    Boundary_interval get_approximation_x() const {
        return Boundary_interval(0.0, 0.0);
    }

    Boundary_interval get_approximation_y() const {
        return Boundary_interval(0.0, 0.0);
    }
    
    void refine_x() const {
    }
    
    void refine_x(int rel_prec) {
    }

    void refine_y() const {
    }
    
    //!@}

}; // class Xy_coordinate_2

template < class AlgebraicCurveKernel_2> 
std::ostream& operator<< (std::ostream& os, 
    const Xy_coordinate_2<AlgebraicCurveKernel_2>& pt) {
    return os;
}

///////////////////////////////////////////////////////////////////////////////

template < class CurveAnalysis_2>
class Status_line_CA_1;

template < class CurveAnalysis_2 >
class Status_line_CA_1_rep {

    // this template argument
    typedef CurveAnalysis_2 Curve_analysis_2;

    // myself
    typedef Status_line_CA_1_rep<Curve_analysis_2> Self;

    // type of x-coordinate
    typedef typename Curve_analysis_2::X_coordinate_1
                X_coordinate_1; 

    // an instance of a size type
    typedef typename Curve_analysis_2::size_type size_type;

    // constructors
public:
    // default constructor ()
    Status_line_CA_1_rep() 
    {   }
    
    //! x-coordinate of event info
    mutable X_coordinate_1 _m_x;

    //! this status line id (# of event or # of interval depending on whether
    //! or not this status line encodes an event)
    size_type _m_index;

    //! underlying curve analysis
    Curve_analysis_2 _m_ca; 
    
     // befriending the handle
    friend class Status_line_CA_1<Curve_analysis_2>;
};

template <class CurveAnalysis_2>
class Status_line_CA_1
      : public ::CGAL::Handle_with_policy<
         Status_line_CA_1_rep<CurveAnalysis_2> > {
public:
    //!@{
    //!\name typedefs

    //! this instance's first template parameter
    //! model of AlgebraicKernel_d_2::CurveAnalysis_2
    typedef CurveAnalysis_2 Curve_analysis_2;
    
    //! this instance's second template parameter
    typedef Status_line_CA_1_rep<CurveAnalysis_2> Rep;

    //! this instance itself
    typedef Status_line_CA_1<Curve_analysis_2> Self;

    //! type of x-coordinate
    typedef typename Curve_analysis_2::X_coordinate_1 X_coordinate_1; 

    typedef typename Curve_analysis_2::Xy_coordinate_2 Xy_coordinate_2;

    typedef typename Curve_analysis_2::size_type size_type;

    //! encodes number of arcs to the left and to the right
    typedef std::pair<size_type, size_type> Arc_pair;

     //! the handle superclass
    typedef ::CGAL::Handle_with_policy< Rep > Base;
    
    //!@}
public:
    //!\name constructors
    //!@{

    /*!\brief
     * Default constructor
     */
    Status_line_CA_1() : 
        Base(Rep()) {   
    }

    /*!\brief
     * copy constructor
     */
    Status_line_CA_1(const Self& p) : 
            Base(static_cast<const Base&>(p)) {  
    }

    /*!\brief
     * constructs from a given represenation
     */
    Status_line_CA_1(Rep rep) : 
        Base(rep) {  
    }
    
    X_coordinate_1 x() const {
        return X_coordinate_1();
    }
    
    Curve_analysis_2 curve_analysis_2() const {
        return Curve_analysis_2();
    }
    
    size_type index() const {
        return static_cast<size_type>(0);
    }
        
    bool covers_line() const {
        return false;
    }

    bool has_f_fy_intersection() const {
        return false;
    }

    bool is_event() const {
        return false;
    }

    size_type number_of_events() const {
        return static_cast<size_type>(0);
    }

    Xy_coordinate_2 algebraic_real_2(size_type j) const {
        return Xy_coordinate_2();
    }
    
    Xy_coordinate_2 xy_coordinate_2(size_type j) const {
        return algebraic_real_2(j);
    }

    Arc_pair number_of_incident_branches(int j) const {
        return Arc_pair(0, 0);
    }

    Arc_pair number_of_branches_approaching_minus_infinity() const {
        return Arc_pair(0, 0);
    }

    Arc_pair number_of_branches_approaching_plus_infinity() const {
        return Arc_pair(0, 0);
    }
  
}; // class Status_line_CA_1

template <class CurveAnalysis_2>
std::ostream& operator<< (std::ostream& os, const
        Status_line_CA_1<CurveAnalysis_2>& cp_line) {
    return os;
}

///////////////////////////////////////////////////////////////////////////////

template < class AlgebraicCurveKernel_2>
class Curve_analysis_2;

template < class AlgebraicCurveKernel_2 >
class Curve_analysis_2_rep {

public:
    // this first template argument
    typedef AlgebraicCurveKernel_2 Algebraic_curve_kernel_2;

    // myself
    typedef Curve_analysis_2_rep<Algebraic_curve_kernel_2> Self;

    typedef typename Algebraic_curve_kernel_2::Curve_pair_2 Curve_pair_2;

    typedef typename Curve_pair_2::Algebraic_curve_2 Curve_2; 
    
    // constructors
public:
    // default constructor ()
    Curve_analysis_2_rep()  
    {  }
    
    // standard constructor
    Curve_analysis_2_rep(const Curve_2& curve) 
    {   }

    mutable Curve_2 _m_curve;
    
    // befriending the handle
    friend class Curve_analysis_2<Algebraic_curve_kernel_2>;
};
    
template <class AlgebraicCurveKernel_2>
class Curve_analysis_2 :
    public ::CGAL::Handle_with_policy<
        Curve_analysis_2_rep<AlgebraicCurveKernel_2> > {
public:
    //!@{
    //! \name typedefs

    //! this instance's first template parameter
    typedef AlgebraicCurveKernel_2 Algebraic_curve_kernel_2;

    //! this instance's second template parameter
    typedef Curve_analysis_2_rep<AlgebraicCurveKernel_2> Rep;
    
    //! x-coordinate type
    typedef typename Algebraic_curve_kernel_2::X_coordinate_1 X_coordinate_1;

    //! x-coordinate type
    typedef typename Algebraic_curve_kernel_2::Xy_coordinate_2 Xy_coordinate_2;

    //! type of a curve
    typedef typename Algebraic_curve_kernel_2::Curve_2 Curve_2;

    //! myself
    typedef Curve_analysis_2<Algebraic_curve_kernel_2> Self;

    //! an instance of a size type
    typedef int size_type;

    //! type of a vertical line
    typedef CGALi::Status_line_CA_1<Self> Status_line_1;
        
    //! the handle superclass
    typedef ::CGAL::Handle_with_policy<Rep> Base;
    
    //!@}
public:
    //!\name Constructors
    //!@{

    //! \brief default constructor
    Curve_analysis_2() : 
        Base(Rep()) {  
    }

    /*!\brief
     * copy constructor
     */
    Curve_analysis_2(const Self& p) : 
        Base(static_cast<const Base&>(p)) {  
    }

    //! \brief constructs a curve analysis from a given \c Curve_2 object
    //!
    //! for safety purposes implicit conversion from \c Curve_2 is disabled
    explicit Curve_analysis_2(const Curve_2& c) : 
        Base(Rep(c)) {  
    }
           
    /*!\brief
     * constructsa curve analysis from a given represenation
     */
    Curve_analysis_2(Rep rep) : 
        Base(rep) {  
    }

    //!@}
public:
    //!\name Access functions
    //!@{

    //! \brief returns the defining polynomial of the analysis
    Curve_2 polynomial_2() const {
        return this->ptr()->_m_curve;
    }

    //! \brief alias for \c polynomial_2()
    Curve_2 curve_2() const
    { 
        return polynomial_2();
    }

    //! \brief returns number of vertical lines that encode an event
    size_type number_of_status_lines_with_event() const {
        return 0;
    }

    Status_line_1 status_line_at_event(size_type i) const {
        return Status_line_1();
    }

    Status_line_1 status_line_of_interval(size_type i) const {
        return Status_line_1();
    }

    Status_line_1 status_line_for_x(X_coordinate_1 x,
        CGAL::Sign perturb = CGAL::ZERO) const {
        return Status_line_1();
    }

    Status_line_1 status_line_at_exact_x(X_coordinate_1 x) const {
        return Status_line_1();
    }
    
    //!@}
}; // class Curve_analysis_2

//////////////////////////////////////////////////////////////////////////////

template < class CurvePairAnalysis_2, class Rep_ > 
class Status_line_CPA_1;

template <class CurvePairAnalysis_2, class Rep>
std::ostream& operator<< (std::ostream&, 
    const Status_line_CPA_1<CurvePairAnalysis_2, Rep>&);

template < class CurvePairAnalysis_2 >
class Status_line_CPA_1_rep {

    // this template argument
    typedef CurvePairAnalysis_2 Curve_pair_analysis_2;

    // myself
    typedef Status_line_CPA_1_rep<Curve_pair_analysis_2> Self;

    // an instance of a size type
    typedef typename Curve_pair_analysis_2::size_type size_type;

    // constructors
public:
    // default constructor ()
    Status_line_CPA_1_rep()  
    {   }

    // stores this status line interval or event index of a curve pair
    size_type _m_index;
    
    // befriending the handle
    friend class Status_line_CPA_1<Curve_pair_analysis_2, Self>;
};

template <class CurvePairAnalysis_2,
      class Rep_ = CGALi::Status_line_CPA_1_rep<CurvePairAnalysis_2> >
class Status_line_CPA_1 : 
    public ::CGAL::Handle_with_policy< Rep_ > 
{
public:
    //!@{
    //!\name typedefs

    //! this instance's first template parameter
    typedef CurvePairAnalysis_2 Curve_pair_analysis_2;
    
    //! this instance's second template parameter
    typedef Rep_ Rep;

    //! this instance itself
    typedef Status_line_CPA_1<Curve_pair_analysis_2, Rep> Self;

    //! type of x-coordinate
    typedef typename Curve_pair_analysis_2::X_coordinate_1 X_coordinate_1; 

    //! an instance of a size type
    typedef typename Curve_pair_analysis_2::size_type size_type;

    //! encodes number of arcs to the left and to the right
    typedef std::pair<size_type, size_type> Arc_pair;

     //! the handle superclass
    typedef ::CGAL::Handle_with_policy< Rep > Base;
    
    //!@}
public:
    //!\name constructors
    //!@{

    Status_line_CPA_1() :
        Base(Rep()) {   
    }

    Status_line_CPA_1(const Self& p) :
            Base(static_cast<const Base&>(p)) {  
    }
        
    /*!\brief
     * constructs from a given represenation
     */
    Status_line_CPA_1(Rep rep) : 
        Base(rep) {  
    }
    
    X_coordinate_1 x() const {
        return X_coordinate_1();
    }
    
    //! returns this vertical line's index (event or interval index)
    size_type index() const {
        return this->ptr()->_m_index;
    }
        
    size_type number_of_events() const {
        return static_cast<size_type>(0);
    }

    size_type event_of_curve(size_type k, bool c) const {
        return static_cast<size_type>(0);
    }

    size_type multiplicity_of_intersection(size_type j) const {
        return static_cast<size_type>(0);
    }

    Arc_pair curves_at_event(size_type j) const {
        return Arc_pair(0, 0);
    }

    bool is_event() const {
        return false;
    }

    bool is_intersection() const {
        return false;
    }
    
    //!@}
}; // class Status_line_CPA_1

template <class CurvePairAnalysis_2, class Rep>
std::ostream& operator<< (std::ostream& os,
        const CGALi::Status_line_CPA_1<CurvePairAnalysis_2, Rep>& cpv_line) {
        
    return os;
}

///////////////////////////////////////////////////////////////////////////////

template < class AlgebraicCurveKernel_2, class Rep_ > 
class Curve_pair_analysis_2;

template < class AlgebraicCurveKernel_2 >
class Curve_pair_analysis_2_rep {

public:
    // this first template argument
    typedef AlgebraicCurveKernel_2 Algebraic_curve_kernel_2;

    // myself
    typedef Curve_pair_analysis_2_rep<Algebraic_curve_kernel_2> Self;

    // type of curve pair
    typedef typename Algebraic_curve_kernel_2::Curve_pair_2 Curve_pair_2;
    
    // type of 1-curve analysis
    typedef typename Algebraic_curve_kernel_2::Curve_analysis_2
        Curve_analysis_2;

    // constructors
public:
    // default constructor ()
    Curve_pair_analysis_2_rep() 
    {   }

    // data
    Curve_analysis_2 _m_ca1, _m_ca2;
    
    // befriending the handle
    friend class Curve_pair_analysis_2<Algebraic_curve_kernel_2, Self>;
};
    
template <class AlgebraicCurveKernel_2,
      class Rep_ = CGALi::Curve_pair_analysis_2_rep<AlgebraicCurveKernel_2> >
class Curve_pair_analysis_2 : public ::CGAL::Handle_with_policy< Rep_ > 
{
public:
    //!@{
    //! \name typedefs

    //! this instance's first template parameter
    typedef AlgebraicCurveKernel_2 Algebraic_curve_kernel_2;

    //! this instance's second template parameter
    typedef Rep_ Rep;

    //! x-coordinate type
    typedef typename Algebraic_curve_kernel_2::X_coordinate_1 X_coordinate_1;

    //! type of a curve point
    typedef typename Algebraic_curve_kernel_2::Xy_coordinate_2 Xy_coordinate_2;

    //! type of a curve pair
    typedef typename Algebraic_curve_kernel_2::Curve_pair_2 Curve_pair_2;

    //! type of a curve
    typedef typename Algebraic_curve_kernel_2::Curve_2 Curve_2;

    //! type of 1-curve analysis
    typedef typename Algebraic_curve_kernel_2::Curve_analysis_2 
            Curve_analysis_2;

    //! an instance of a size type
    typedef typename Curve_analysis_2::size_type size_type;

    //! myself
    typedef Curve_pair_analysis_2<Algebraic_curve_kernel_2, Rep> Self;

    //! type of a vertical line
    typedef CGALi::Status_line_CPA_1<Self> Status_line_1;
        
    //! the handle superclass
    typedef ::CGAL::Handle_with_policy<Rep> Base;

    //!@}
public:
    //!\name Constructors
    //!@{

    //! \brief default constructor
    Curve_pair_analysis_2() : 
        Base(Rep()) {  
    }

    /*!\brief
     * copy constructor
     */
    Curve_pair_analysis_2(const Self& p) : 
        Base(static_cast<const Base&>(p)) {  
    }

    Curve_pair_analysis_2(const Curve_analysis_2& ca1,
        const Curve_analysis_2& ca2) : 
            Base(Rep()) {  
    }
 
    Curve_pair_analysis_2(Rep rep) :
        Base(rep) {  
    }

    Curve_analysis_2 curve_analysis(bool c) const {
        return this->ptr()->_m_ca1;
    }

    size_type number_of_status_lines_with_event() const {
        return static_cast<size_type>(0);
    }

    size_type event_of_curve_analysis(size_type i, bool c) const {
        return static_cast<size_type>(0);
    }

    Status_line_1 status_line_at_event(size_type i) const {
        return Status_line_1();
    }

    Status_line_1 status_line_of_interval(size_type i) const {
        return Status_line_1();
    }

    Status_line_1 status_line_for_x(X_coordinate_1 x,
            CGAL::Sign perturb = CGAL::ZERO) const {
        return Status_line_1();
    }

    Status_line_1& status_line_at_exact_x(X_coordinate_1 x) const {
        return Status_line_1();
    }

    //!@}
}; // class Curve_pair_analysis_2

} // namespace CGALi

//////////////////////////////////////////////////////////////////////////////

class Simple_algebraic_kernel_2 {

// for each predicate functor defines a member function returning an instance
// of this predicate
#define CGAL_Algebraic_Kernel_pred(Y,Z) \
    Y Z() const { return Y(); }

// the same for construction functors
#define CGAL_Algebraic_Kernel_cons(Y,Z) CGAL_Algebraic_Kernel_pred(Y,Z)
    
private:
    //! \name wrapping types
    //!@{

    //! type of an internal curve pair
    typedef CGALi::Curve_pair_2_model<CGALi::Curve_2_model>
         Internal_curve_pair_2;

    //! type of an internal curve
    typedef Internal_curve_pair_2::Algebraic_curve_2 Internal_curve_2;

    //! type of internal x_coordinate
    typedef Internal_curve_2::X_coordinate Internal_x_coordinate;
    
    //! type of internal coefficient
    typedef Internal_curve_2::Coefficient Internal_coefficient;

    //!@}
public:
    //! \name types and functors for \c ACK_2< >
    //!@{
    
    //! myself
    typedef Simple_algebraic_kernel_2  Self;
    
    //! type of curve pair
    typedef Internal_curve_pair_2 Curve_pair_2;

    //! type of single curve
    typedef Internal_curve_2 Curve_2;

    //! type of x-coordinate
    typedef Internal_x_coordinate X_coordinate_1;

    typedef Internal_curve_2::Boundary Boundary;
        
    //! new CGAL univariate polynomial type (_CGAL postfix is temporary to
    //! avoid type clashes with \c Polynomial_2 type defined later
    typedef CGAL::Polynomial<int> Polynomial_1_CGAL;

    //! new CGAL bivariate polynomial type
    typedef CGAL::Polynomial<Polynomial_1_CGAL> Polynomial_2_CGAL;

    //!@}
    //! \name public functors and predicates
    //!@{
       
    //! \brief default constructor
    Simple_algebraic_kernel_2() 
    {  }
    
    //! \brief constructs \c Curve_2 object, uses caching if appropriate
    struct Construct_curve_2 :
            public Unary_function< Polynomial_2_CGAL, Curve_2 >
    {
        //! \brief constructs an object from \c Algebraic_curve_kernel_2 type
        //! no default constructor provided
        Construct_curve_2(/*Self *pkernel_2*/) 
        {  }
            
        Curve_2 operator()(const Polynomial_2_CGAL& f) const
        {
            return Curve_2();
        }
    };
    CGAL_Algebraic_Kernel_cons(Construct_curve_2, construct_curve_2_object);
    
    //! type of a curve point 
    typedef CGALi::Xy_coordinate_2<Self> Xy_coordinate_2;
    
    struct X_real_traits_1 {
        typedef double Boundary;
    };

    struct Y_real_traits_1 {
        typedef double Boundary;
    };
    
    //! returns the first coordinate of \c Xy_coordinate_2
    struct Get_x_2 :
        public Unary_function<Xy_coordinate_2, X_coordinate_1> {
        
        X_coordinate_1 operator()(const Xy_coordinate_2& xy) const {
            return xy.x();
        }
    };
    CGAL_Algebraic_Kernel_cons(Get_x_2, Get_x_2_object);
    
    //! returns the second coordinate of \c Xy_coordinate_2
    struct Get_y_2 :
        public Unary_function<Xy_coordinate_2, X_coordinate_1> {
        
        X_coordinate_1 operator()(const Xy_coordinate_2& xy) const {
            return xy.y();
        }
    };
    CGAL_Algebraic_Kernel_cons(Get_y_2, Get_y_2_object);
    
    struct Refine_x_2 :
        public Unary_function<Xy_coordinate_2, void> {
      
        void operator()(const Xy_coordinate_2& r) const {  }
        
        void operator()(Xy_coordinate_2& r, int rel_prec) const {  }
    };
    CGAL_Algebraic_Kernel_pred(Refine_x_2, refine_x_2_object);
    
    struct Refine_y_2 :
        public Unary_function<Xy_coordinate_2, void> {
      
        void operator()(const Xy_coordinate_2& r) const {  }
        
        void operator()(Xy_coordinate_2& r, int rel_prec) const {  }
    };
    CGAL_Algebraic_Kernel_pred(Refine_y_2, refine_y_2_object);
    
    //! computes the current lower boundary of the first coordinate of \c r
    struct Lower_boundary_x_2 {
       
        typedef Xy_coordinate_2 agrument_type;
        typedef Boundary result_type;
            
        result_type operator()(const Xy_coordinate_2& r) {
            return static_cast<result_type>(0); 
        }
    };
    CGAL_Algebraic_Kernel_cons(Lower_boundary_x_2, lower_boundary_x_2_object);
    
    //! computes the current upper boundary of the first coordinate of \c r
    struct Upper_boundary_x_2 {
       
        typedef Xy_coordinate_2 agrument_type;
        typedef Boundary result_type;
            
        result_type operator()(const Xy_coordinate_2& r) {
            return static_cast<result_type>(0);  
        }
    };
    CGAL_Algebraic_Kernel_cons(Upper_boundary_x_2, upper_boundary_x_2_object);
    
    //! computes the current lower boundary of the second coordinate of \c r
    struct Lower_boundary_y_2 {
       
        typedef Xy_coordinate_2 agrument_type;
        typedef Boundary result_type;
            
        result_type operator()(const Xy_coordinate_2& r) {
            return static_cast<result_type>(0);  
        }
    };
    CGAL_Algebraic_Kernel_cons(Lower_boundary_y_2, lower_boundary_y_2_object);
    
    //! computes the current lower boundary of the second coordinate of \c r
    struct Upper_boundary_y_2 {
       
        typedef Xy_coordinate_2 agrument_type;
        typedef Boundary result_type;
            
        result_type operator()(const Xy_coordinate_2& r) {
            return static_cast<result_type>(0); 
        }
    };
    CGAL_Algebraic_Kernel_cons(Upper_boundary_y_2, upper_boundary_y_2_object);
    
    //! returns the number of boundary type in-between x-coordinates of two
    //! Xy_coordinate_2 objects
    struct Boundary_between_x_2 {
       
        typedef Xy_coordinate_2 first_agrument_type;
        typedef Xy_coordinate_2 second_agrument_type;
        typedef Boundary result_type;
            
        result_type operator()(const Xy_coordinate_2& r1, 
                const Xy_coordinate_2& r2) const {
            return static_cast<result_type>(0); 
        }
    };
    CGAL_Algebraic_Kernel_cons(Boundary_between_x_2, 
            boundary_between_x_2_object);
            
    //! returns the number of boundary type in-between y-coordinates of two
    //! Xy_coordinate_2 objects
    struct Boundary_between_y_2 {
       
        typedef Xy_coordinate_2 first_agrument_type;
        typedef Xy_coordinate_2 second_agrument_type;
        typedef Boundary result_type;
            
        result_type operator()(const Xy_coordinate_2& r1, 
                const Xy_coordinate_2& r2) const {
            return static_cast<result_type>(0); 
        }
    };
    CGAL_Algebraic_Kernel_cons(Boundary_between_y_2, 
            boundary_between_y_2_object);
    
    //! \brief comparison of x-coordinates 
    struct Compare_x_2 :
         public Binary_function<X_coordinate_1, X_coordinate_1, 
                Comparison_result > {

        Comparison_result operator()(const X_coordinate_1& x1, 
                                         const X_coordinate_1& x2) const {
            return CGAL::EQUAL;
        }
        Comparison_result operator()(const Xy_coordinate_2& xy1, 
                                         const Xy_coordinate_2& xy2) const {
            return CGAL::EQUAL;
        }
    };
    CGAL_Algebraic_Kernel_pred(Compare_x_2, compare_x_2_object);

    //! \brief comparison of y-coordinates of two points
    struct Compare_y_2 :
        public Binary_function< Xy_coordinate_2, Xy_coordinate_2, 
                Comparison_result > {
        
        Comparison_result operator()(const Xy_coordinate_2& xy1, 
                                     const Xy_coordinate_2& xy2) const {
            return CGAL::EQUAL;
        }
    };
    CGAL_Algebraic_Kernel_pred(Compare_y_2, compare_y_2_object);
    
    //! lexicographical comparison of two objects of type \c Xy_coordinate_2
    //!
    //! \c equal_x specifies that only y-coordinates need to be compared
    struct Compare_xy_2 :
          public Binary_function<Xy_coordinate_2, Xy_coordinate_2, 
                Comparison_result > 
    {
        Comparison_result operator()(const Xy_coordinate_2& xy1, 
             const Xy_coordinate_2& xy2, bool equal_x = false) const {
             
             return CGAL::EQUAL;
        }
    };
    CGAL_Algebraic_Kernel_pred(Compare_xy_2, compare_xy_2_object);

    //! \brief checks whether curve has only finitely many self-intersection
    //! points, i.e., it has no self-overlapped continuous parts
    //!
    //! for algerbaic curves this means that supporting polynomial is 
    //! square-free
    struct Has_finite_number_of_self_intersections_2 :
            public Unary_function< Polynomial_2_CGAL, bool > {

        bool operator()(const Polynomial_2_CGAL& p) const {
            return true; //is_square_free(p);
        }
    };
    CGAL_Algebraic_Kernel_pred(Has_finite_number_of_self_intersections_2, 
            has_finite_number_of_self_intersections_2_object);
            
    //! \brief checks whether a curve pair has finitely many intersections,
    //! in other words, whether two curves have no continuous common part
    //!
    //! in case of algerbaic curves: checks whether supporting polynomials are
    //! coprime
    struct Has_finite_number_of_intersections_2 :
            public Binary_function< Curve_2, Curve_2, bool > { 
               
        bool operator()(const Curve_2& c1, const Curve_2& c2) const {
            return true;
        }
    };
    CGAL_Algebraic_Kernel_pred(Has_finite_number_of_intersections_2, 
            has_finite_number_of_intersections_2_object);
    
    //! set of various curve and curve pair decomposition functions
    struct Decompose_2 {
    
        //! default constructor
        Decompose_2(/*Self *pkernel_2*/)  
        {  }

        Polynomial_2_CGAL operator()(const Polynomial_2_CGAL& p) {
            return p;
        }

        template< class OutputIterator1, class OutputIterator2 >
        int operator()( const Curve_2& c, OutputIterator1 fit, 
                        OutputIterator2 mit ) const {
                        
            return 0;
        }
        
        template < class OutputIterator >
        bool operator()(const Curve_2& c1, const Curve_2& c2,
            OutputIterator oi1, OutputIterator oi2, OutputIterator oib) {
           
            return false;
        }
    private:
        //! pointer to Algebraic_curve_kernel_2 (for caching issues)
        /*Self *_m_pkernel_2; */
    };
    CGAL_Algebraic_Kernel_cons(Decompose_2, decompose_2_object);
    
    //!@}
public:
    //! \name types and functors for \c GPA_2<Algebraic_kernel_2>
    //!@{
    
    typedef Curve_2 Polynomial_2; 
    
    typedef Construct_curve_2 Construct_polynomial_2_;

    typedef X_coordinate_1 Algebraic_real_1;
    typedef Xy_coordinate_2 Algebraic_real_2;
    
    typedef Has_finite_number_of_self_intersections_2 Is_square_free_2;
    typedef Has_finite_number_of_intersections_2 Is_coprime_2;

    typedef Decompose_2 Make_square_free_2;
    typedef Decompose_2 Square_free_factorization;
    typedef Decompose_2 Make_coprime_2;
    
    //! \brief computes the derivative w.r.t. the first (innermost) variable
    struct Derivative_x_2 : 
        public Unary_function< Polynomial_2_CGAL, Polynomial_2_CGAL > {
        
        Polynomial_2_CGAL operator()(const Polynomial_2_CGAL& p) const {
            return p;
        }
    };
    CGAL_Algebraic_Kernel_cons(Derivative_x_2, derivative_x_2_object);

    //! \brief computes the derivative w.r.t. the first (outermost) variable
    struct Derivative_y_2 :
        public Unary_function< Polynomial_2_CGAL, Polynomial_2_CGAL > {
        
        Polynomial_2_CGAL operator()(const Polynomial_2_CGAL& p) const  {
            return p;
        }
    };
    CGAL_Algebraic_Kernel_cons(Derivative_y_2, derivative_y_2_object);

    struct X_critical_points_2 {
       
        template <class OutputIterator>
        OutputIterator operator()(const Polynomial_2& p, 
                OutputIterator oi) const {
            return oi;
        }
        
        //! \brief computes the ith x-critical point of polynomial \c p
        Xy_coordinate_2 operator()(const Polynomial_2& p, int i) const {
            return Xy_coordinate_2();
        }
    };
    CGAL_Algebraic_Kernel_cons(X_critical_points_2,
        x_critical_points_2_object);
    
    struct Y_critical_points_2 {
    
        //! \brief copies in the output iterator the y-critical points of
        //! polynomial \c p as objects of type \c Xy_coordinate_2
        template <class OutputIterator>
        OutputIterator operator()(const Polynomial_2& p, 
            OutputIterator oi) const {
            return oi;
        }
        
        //! \brief computes the ith y-critical point of polynomial \c p
        Xy_coordinate_2 operator()(const Polynomial_2& p, int i) const {
            return Xy_coordinate_2();
        }
    };
    CGAL_Algebraic_Kernel_cons(Y_critical_points_2,
        y_critical_points_2_object);

    /*!\brief 
     * computes the sign of a bivariate polynomial \c p evaluated at the root 
     * \c r of a system of two bivariate polynomial equations
     *
     * returns a value convertible to \c CGAL::Sign
     */
    struct Sign_at_2 :
        public Binary_function< Polynomial_2, Xy_coordinate_2, Sign > {

        Sign operator()(const Polynomial_2& p, const Xy_coordinate_2& r) const
        {
            return CGAL::ZERO;
        }
    };
    CGAL_Algebraic_Kernel_pred(Sign_at_2, sign_at_2_object);

    struct Solve_2 {
    
        template <class OutputIteratorRoots, class OutputIteratorMult>
        std::pair<OutputIteratorRoots, OutputIteratorMult>
            operator()(const Polynomial_2& p1, const Polynomial_2& p2,
                OutputIteratorRoots roots, OutputIteratorMult mults) const
        {
            return std::make_pair(roots, mults);
        }
    };
    CGAL_Algebraic_Kernel_cons(Solve_2, solve_2_object);

#undef CGAL_Algebraic_Kernel_pred    
#undef CGAL_Algebraic_Kernel_cons 
    
    //!@}
public:
    //! \name types and functors for \c GPA_2< both >
    //!@{
   
    //! type of 1-curve analysis
    typedef CGALi::Curve_analysis_2<Self> Curve_analysis_2; 

    //! type of 2-curve analysis
    typedef CGALi::Curve_pair_analysis_2<Self> Curve_pair_analysis_2; 
    
    //!@}
      
}; // class Algebraic_curve_kernel_2

CGAL_END_NAMESPACE

#endif // CGAL_CKVA_2_SIMPLE_MODELS_H
