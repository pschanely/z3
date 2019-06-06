#include "math/lp/nla_core.h"
#include "math/interval/interval_def.h"
#include "math/lp/nla_intervals.h"
namespace nla {

bool intervals::check() {
    // m_region.reset();
    // for (auto const& m : m_core->emons()) {
    //     if (!check(m)) {
    //         return false;
    //     }
    // }
    // for (auto const& t : ls().terms()) {
    //     if (!check(*t)) {
    //         return false;
    //     }
    // }
    return true;
}

bool intervals::check(monomial const& m) {
    interval a, b, c, d;
    m_imanager.set(a, rational(1).to_mpq());
    set_interval(m.var(), d);
    if (m_imanager.lower_is_inf(d) && m_imanager.upper_is_inf(d)) {
        return true;
    }
    for (lpvar v : m.vars()) {
        // TBD allow for division to get range of a
        // m = a*b*c, where m and b*c are bounded, then interval for a is m/b*c
        if (m_imanager.lower_is_inf(a) && m_imanager.upper_is_inf(a)) {
            return true;
        }
        // TBD: deal with powers v^n interval instead of multiplying v*v .. * v
        set_interval(v, b);
        interval_deps deps;
        m_imanager.mul(a, b, c, deps);
        m_imanager.set(a, c);
        m_config.set_deps(a, b, deps, a);
    }
    if (m_imanager.before(a, d)) {
        svector<lp::constraint_index> cs;
        m_dep_manager.linearize(a.m_upper_dep, cs);
        m_dep_manager.linearize(d.m_lower_dep, cs);
        for (auto ci : cs) {
            (void)ci;
            SASSERT(false);
            //expl.push_justification(ci);
        }
        // TBD conflict
        return false;
    }
    if (m_imanager.before(d, a)) {
        svector<lp::constraint_index> cs;
        m_dep_manager.linearize(a.m_lower_dep, cs);
        m_dep_manager.linearize(d.m_upper_dep, cs);
        for (auto ci : cs) {
            (void)ci;
            SASSERT(false); //expl.push_justification(ci);
        }
        // TBD conflict
        return false;
    }
    // could also perform bounds propagation:
    // a has tighter lower/upper bound than m.var(), 
    // -> transfer bound to m.var()
    // all but one variable has bound
    // -> transfer bound to that variable using division
    return true;
}

void intervals::set_interval(lpvar v, interval& b) {
    lp::constraint_index ci;
    rational val;
    bool is_strict;
    if (ls().has_lower_bound(v, ci, val, is_strict)) {            
        m_config.set_lower(b, val);
        m_config.set_lower_is_open(b, is_strict);
        m_config.set_lower_is_inf(b, false);
        b.m_lower_dep = mk_dep(ci);
    }
    else {
        m_config.set_lower_is_open(b, true);
        m_config.set_lower_is_inf(b, true);
        b.m_lower_dep = nullptr;
    }
    if (ls().has_upper_bound(v, ci, val, is_strict)) {
        m_config.set_upper(b, val);
        m_config.set_upper_is_open(b, is_strict);
        m_config.set_upper_is_inf(b, false);
        b.m_upper_dep = mk_dep(ci);
    }
    else {
        m_config.set_upper_is_open(b, true);
        m_config.set_upper_is_inf(b, true);
        b.m_upper_dep = nullptr;
    }
}
    
intervals::ci_dependency *intervals::mk_dep(lp::constraint_index ci) {
    return m_dep_manager.mk_leaf(ci);
}
    
bool intervals::check(lp::lar_term const& t) {
    // convert term into factors for improved precision
    return true;
}

lp::impq intervals::get_upper_bound_of_monomial(lpvar j) const {
    SASSERT(false);
    throw;
}

lp::impq intervals::get_lower_bound_of_monomial(lpvar j) const {
    SASSERT(false);
    throw;
}

bool intervals::product_has_upper_bound(int sign, const svector<lpvar>&) const {
    interval a;
    SASSERT(false);
}

bool intervals::monomial_has_lower_bound(lpvar j) const {
    const monomial& m = m_core->emons()[j];
    return product_has_upper_bound(1, m.vars());
}

bool intervals::monomial_has_upper_bound(lpvar j) const {
    const monomial& m = m_core->emons()[j];
    return product_has_upper_bound(-1, m.vars());
}
lp::lar_solver& intervals::ls() { return m_core->m_lar_solver; }
}
template class interval_manager<nla::intervals::im_config>;