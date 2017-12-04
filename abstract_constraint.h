#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <pblib/pb2cnf.h>

namespace abstract_constraint {

  class constraint {
  private:

  protected:
    std::string name;
    std::vector< int32_t > variables;

  public:
    constraint();
    constraint(std::vector< int32_t >& variables);
    void add_variable(int32_t);
    const std::vector< int32_t >& get_variables();
    virtual void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free);
  };

  class at_most_k : public constraint {
  private:
    int k;
  public:
    at_most_k() {}
    at_most_k(int k, std::vector< int32_t >& variables);
    int get_k();
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free);
  };

  class exactly_k : public constraint {
  private:
    int k;
  public:
    exactly_k() {}
    exactly_k(int k, std::vector< int32_t >& variables);
    int get_k();
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free);
  };

  class not_exactly_one : public constraint {
  private:
  public:
    not_exactly_one() {}
    not_exactly_one(std::vector< int32_t >& variables);
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free);
  };

  class cnfclause : public constraint {
  private:
  public:
    cnfclause() {}
    cnfclause(std::vector< int32_t >& variables);
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free);
  };


};
