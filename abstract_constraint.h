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
    std::vector< std::string > variables;

  public:
    constraint();
    constraint(std::vector< std::string >& variables);
    void add_variable(std::string&);
    const std::vector< std::string >& get_variables();
    virtual void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free, std::map< std::string, int >& name2id);
    std::vector< int32_t > get_literals(std::map< std::string, int>&);
  };

  class at_most_k : public constraint {
  private:
    int k;
  public:
    at_most_k() {}
    at_most_k(int k, std::vector< std::string >& variables);
    int get_k();
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free, std::map< std::string, int >& name2id);
  };
  std::ostream& operator<<(std::ostream& os, at_most_k& amk);

  class exactly_k : public constraint {
  private:
    int k;
  public:
    exactly_k() {}
    exactly_k(int k, std::vector< std::string >& variables);
    int get_k();
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free, std::map< std::string, int >& name2id);
  };
  std::ostream& operator<<(std::ostream& os, exactly_k& exk);

  class not_exactly_one : public constraint {
  private:
  public:
    not_exactly_one() {}
    not_exactly_one(std::vector< std::string >& variables);
    void to_sat(std::vector< std::vector< int32_t > >& formula,
    int& first_free, std::map< std::string, int >& name2id);
  };
  std::ostream& operator<<(std::ostream& os, not_exactly_one& neo);



};
