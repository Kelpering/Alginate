#include <iostream>
#include "Alginate.hpp"
#include "Alginate_old.hpp"

// int main()
// {
//     BigNum x = {std::vector<uint32_t> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, false};
//     BigNum y = {std::vector<uint32_t> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, false};

//     (x*x).print_debug();
// }

int main()
{
    uint32_t* num_array1 = new uint32_t[2] {0, 0xFFFFFFFF};
    uint32_t* num_array2 = new uint32_t[2] {0, 0xFFFFFFFF};
    // uint32_t* num_array3 = new uint32_t[2] {0};
    AlgInt test1 = {num_array1, 2, false};
    AlgInt test2 = {num_array2, 2, false};
    // AlgInt test3 = {num_array3, 2, false};

    test1.print_debug();
    test2.print_debug();

    AlgInt::k_branch** workspace;
    size_t level = AlgInt::prepare_mul_workspace(test1, test2, workspace);

    std::cout << "level: " << level << "\n";

    AlgInt::internal_mul(workspace, level);

    workspace[level]->ret->print_debug("ret", true);



    // Variable de-allocation
    test1.destroy();
    test2.destroy();
    // test3.destroy();

    AlgInt::destroy_mul_workspace(workspace, level);
    

    return 0;
}