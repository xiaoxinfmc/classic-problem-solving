#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class Employee {
public:
  Employee(int age, string name, int seq) : age(age), seq(seq), name(name) {}
  int age; int seq; string name;
  friend bool operator< (const Employee & lemp, const Employee & remp) {
    if (lemp.age == remp.age) { return lemp.name < remp.name; }
    else { return lemp.age < remp.age; }
  }
  friend bool operator== (const Employee & lemp, const Employee & remp) {
    return (lemp.age == remp.age && lemp.name == remp.name);
  }
};


int main() {
  Employee x1(10, "xf", 1); Employee x2(10, "xf", 2);
  Employee x3(11, "xh", 3); Employee x4(90, "xa", 4);
  Employee x5(10, "xf", 5); Employee x6(10, "xf", 6);
  Employee x7(39, "xk", 7);

  Employee stuff_arr[] = { x1, x2, x3, x4, x5, x6, x7 };

  vector<Employee> stuff_vec(
    stuff_arr, stuff_arr + sizeof(stuff_arr) / sizeof(stuff_arr[0])
  );
  stable_sort(stuff_vec.begin(), stuff_vec.end());
  /* sort(stuff_vec.begin(), stuff_vec.end()); */

  cout << "==>> before sort:" << endl;
  for (auto & iter : stuff_vec) {
    cout << iter.age << " " << iter.name << " " << iter.seq << endl;
  }
  cout << "<<== afters sort:" << endl;
  for (vector<Employee>::const_iterator itr = stuff_vec.begin();
       itr != stuff_vec.end(); itr++) {
    cout << itr->age << " " << itr->name << " " << itr->seq << endl;
  }

  unordered_map<string, double> mymap;
  unordered_map<string, double>::const_iterator iter;

  mymap.insert(make_pair<string, double>("mom", 5.4));
  mymap["mom"] = 5.7;
  string input; getline(cin, input);

  iter = mymap.find(input);
  if (mymap.end() == iter){ cout << "not found" << endl; }
  else { cout << iter->first << " is " << iter->second << endl; }

  for (auto & bio : mymap) {
    cout << bio.first << " is " << bio.second << endl;
  }

  cout << "bio-size: " << mymap.size() << endl;
  mymap.erase("mom");
  cout << "after erase bio-size: " << mymap.size() << endl;
  mymap.clear();
  cout << "after clear bio-size: " << mymap.size() << endl;

  return 0;
}
