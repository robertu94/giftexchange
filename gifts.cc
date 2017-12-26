#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

std::map<std::string, std::vector<std::string>>
load_families(std::istream& infile)
{
  std::map<std::string, std::vector<std::string>> families;
  for (std::string line; std::getline(infile, line);) {
    if (!line.empty() && line.front() == '#')
      continue;
    std::vector<std::string> family_members;
    std::istringstream family(line);
    std::string family_name;
    std::string member;
    family >> family_name;
    while (family >> member) {
      family_members.emplace_back(member);
    }
    families.emplace(family_name, family_members);
  }
  return families;
}

std::set<std::pair<std::string, std::string>>
load_restrictions(std::istream& infile)
{
  std::set<std::pair<std::string, std::string>> restrictions;

  for (std::string line; std::getline(infile, line);) {
    if (!line.empty() && line.front() == '#')
      continue;
    std::istringstream restriction(line);
    std::string giver, receiver;
    restriction >> giver >> receiver;
    restrictions.emplace(giver, receiver);
  }
  return restrictions;
}

int
main(int argc, char* argv[])
{
  using namespace std;
  std::ifstream families_file(argv[1]);
  map<string, vector<string>> const& family_members =
    load_families(families_file);

  std::ifstream restrictions_file(argv[2]);
  set<pair<string, string>> const& last_year =
    load_restrictions(restrictions_file);

  if (argc >= 4) {
    std::cout << "families " << argv[1] << std::endl;
    for (auto const& family : family_members) {
      std::cout << family.first << " ";
      std::copy(std::begin(family.second),
                std::end(family.second),
                std::ostream_iterator<std::string>(std::cout, " "));
      std::cout << std::endl;
    }
    std::cout << "restrictions " << argv[2] << std::endl;
    for (auto const & [ giver, reciever ] : last_year) {
      std::cout << giver << " " << reciever << std::endl;
    }
    std::cout << "matchings" << std::endl;
  }

  vector<string> names;
  names.reserve(last_year.size());

  typedef boost::
    adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS>
      Traits;
  typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>
    Graph;

  Graph graph;
  map<string, string> family_of;
  map<string, typename Traits::vertex_descriptor> name_to_giver;
  map<string, typename Traits::vertex_descriptor> name_to_reciver;
  map<typename Traits::vertex_descriptor, std::string> vertex_to_name;
  set<typename Traits::vertex_descriptor> givers;
  for (auto const & [ family, members ] : family_members) {
    for (auto const& member : members) {
      names.emplace_back(member);
      family_of.emplace(member, family);
      name_to_giver[member] = boost::add_vertex(graph);
      name_to_reciver[member] = boost::add_vertex(graph);
      vertex_to_name[name_to_giver[member]] = member;
      vertex_to_name[name_to_reciver[member]] = member;
      givers.emplace(name_to_giver[member]);
    }
  }

  for (auto const& giver : names) {
    for (auto const& reciever : names) {
      if (giver == reciever)
        continue;
      if (family_of.at(giver) == family_of.at(reciever))
        continue;
      if (auto it = last_year.find({ giver, reciever }); it != last_year.end())
        continue;
      boost::add_edge(name_to_giver[giver], name_to_reciver[reciever], graph);
    }
  }

  vector<typename Traits::vertex_descriptor> mate(num_vertices(graph));
  boost::checked_edmonds_maximum_cardinality_matching(graph, &mate[0]);
  for (auto[b, e] = boost::vertices(graph); b != e; b++) {
    if (mate[*b] != boost::graph_traits<Graph>::null_vertex()) {
      if (givers.find(*b) != givers.end()) {
        std::cout << vertex_to_name[*b] << " -> " << vertex_to_name[mate[*b]]
                  << std::endl;
      }
    }
  }

  return 0;
}
