#pragma once
#include <iostream>
#include <fstream>
#include <vector>

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
  };

  class at_most_k : public constraint {
  private:
    int k;
  public:
    at_most_k() {}
    at_most_k(int k, std::vector< std::string >& variables);
    int get_k();
  };
  std::ostream& operator<<(std::ostream& os, at_most_k& amk);

  class exactly_k : public constraint {
  private:
    int k;
  public:
    exactly_k() {}
    exactly_k(int k, std::vector< std::string >& variables);
    int get_k();
  };
  std::ostream& operator<<(std::ostream& os, exactly_k& exk);

  class not_exactly_one : public constraint {
  private:
  public:
    not_exactly_one() {}
    not_exactly_one(std::vector< std::string >& variables);
  };
  std::ostream& operator<<(std::ostream& os, not_exactly_one& neo);



};
