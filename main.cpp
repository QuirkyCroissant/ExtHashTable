#include <iostream>
#include <cassert>
#include "ADS_set.h"

int main() {
    // Create an ADS_set instance
    ADS_set<int> data_structure;



    // Test insertion
    data_structure.insert(10);
    auto result = data_structure.insert(20);

    assert(result.second == true);
    assert(data_structure.size() == 2);

    result = data_structure.insert(10);

    // should be false, element 10 already exists
    // and Size still 2
    assert(result.second == false); 
    assert(data_structure.size() == 2);



    // Test finding elements
    auto it = data_structure.find(10);
    assert(it != data_structure.end());

    it = data_structure.find(30);

    // should not be able to find 30
    assert(it == data_structure.end()); 

    // Test erasing elements
    size_t erased = data_structure.erase(10);
    
    assert(erased == 1);
    // size should be 1
    assert(data_structure.size() == 1);

    erased = data_structure.erase(30);
    // Should have erased 0 elements and still size 1
    assert(erased == 0);
    assert(data_structure.size() == 1);



    // Test clear
    data_structure.clear();
    // size should be zero now after clearance
    assert(data_structure.size() == 0);

    // Test inserting from initializer list & Size should be 5
    data_structure.insert({1, 2, 3, 4, 5});
    assert(data_structure.size() == 5); 
    


    // Test copy constructor
    ADS_set<int> data_structure_copy(data_structure);
    assert(data_structure_copy.size() == 5);



    // Test assignment operator
    ADS_set<int> data_structure_assign;
    data_structure_assign = data_structure;
    assert(data_structure_assign.size() == 5); // Size should be 5



    // Test swap
    ADS_set<int> data_structure_swap;
    data_structure_swap.insert({6, 7, 8});
    assert(data_structure_swap.size() == 3);

    swap(data_structure, data_structure_swap);
    assert(data_structure.size() == 3);
    assert(data_structure_swap.size() == 5);



    // Test iterator
    std::cout << "Data structure set contains: ";
    for (const auto &elem : data_structure) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;



    // Dump the set's internal state for debugging
    data_structure.dump();

    // Test inserting a large amount of data
    const int large_test_size = 1000000;
    ADS_set<int> large_data_structure;
    for (int i = 0; i < large_test_size; ++i) {
        large_data_structure.insert(i);
    }



    // Verify the large set size
    assert(large_data_structure.size() == large_test_size);

    // Verify some elements in the large set
    // Should find element 0
    assert(large_data_structure.find(0) != large_data_structure.end());
    // find the middle element
    assert(large_data_structure.find(large_test_size / 2) != large_data_structure.end()); 
    // find the last element
    assert(large_data_structure.find(large_test_size - 1) != large_data_structure.end()); 

    std::cout << "All tests passed!" << std::endl;

    return 0;
}
