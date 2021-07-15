#include "numlist.h"

// Partitioning functions
// Serial partition
unsigned int NumList::partition(vector<int>& keys, unsigned int low, 
                                unsigned int high)
{
    // Use the last element as the pivot
    int pivot = keys[high];

    int i = low - 1; // index of smaller element

    // temporary variable used for swapping
    int temp;

    for(int j = low; j < high; j++)
    {
	    // if current element is smaller than the pivot
	    if(keys[j] < pivot)
	    {
		    i++;
		    // swapping keys[i] and keys[j]
		    temp = keys[i];
		    keys[i] = keys[j];
		    keys[j] = temp;

	    }
    }

    // swapping keys[i + 1] and keys[high]
    temp = keys[i + 1];
    keys[i + 1] = keys[high];
    keys[high] = temp;

    return (i + 1);
}


// Parallel partition
unsigned int NumList:: partition_par(vector<int>& keys, unsigned int low,
                                     unsigned int high)
{
    // Use the last element as the pivot
    int pivot = keys[high];

    // There should be two #pragmas to parallelize the loop
    // First loop is calculating the lt and gt arrays
    unsigned int arr_size = keys.size();

    // declare lt and gt (less than and greater than pivot) and initialize to 0
    unsigned int lt[arr_size] = {0};
    unsigned int gt[arr_size] = {0};

    // declare prefix sums (lt_prefix[i] = lt_prefix[i - 1] + lt[i]) and init to 0
    unsigned int lt_prefix[arr_size] = {0};
    unsigned int gt_prefix[arr_size] = {0};

    
    #pragma omp parallel for 
    // set lt and gt, if same as pivot, keep 0 
    for(unsigned int j = low; j <= high; j++)
    {
	    // lt and gt (dervived from keys), pivot is at high
	    if(keys[j] <= pivot and j != high)
	    {
		    lt[j] = 1;

	    }
	    else if(keys[j] > pivot)
	    {
		    gt[j] = 1;
	    }
    }

    // calculating prefix sums
    for(unsigned int j = low; j <=high; j++) 
    {

	   // prefix sums (derived from lt and gt)
	   if(j > low)
	   {
		   lt_prefix[j] = lt_prefix[j - 1] + lt[j];
		   gt_prefix[j] = gt_prefix[j - 1] + gt[j];
	   }
	   else
	   {
		   lt_prefix[j] = lt[j];
		   gt_prefix[j] = gt[j];
	   }
    }


    // partitioning based on prefix and lt and gt
    unsigned int prefix_index = 0;

    // where pivot needs to be in array
    unsigned int pivot_index = lt_prefix[high] + low;

    // temporarily holds values in correct index. These values will be copied back into
    // the original vector afterword
    vector<int> holder = keys;
    unsigned int p_index[arr_size] = {0};

    // calculating correct indicies via prefix and lt, gt
    for(unsigned int j = low; j <= high; j++)
    {
	    if(lt[j] == 1)
	    {
		    prefix_index = lt_prefix[j] - lt[j] + low;
	    }
	    else if(gt[j] == 1) 
	    {
		    // added pivot_index because these are right of pivot
		    prefix_index = gt_prefix[j] - gt[j] + pivot_index + 1;
	    }
	    else 
	    {
		    // pivot will always be in pivot index
		    prefix_index = pivot_index;
	    }

	    p_index[j] = prefix_index;
    }

    // copying values into correct position of array
    #pragma omp parallel for
    for(unsigned int j = low; j <=high; j++)
    {
	    holder[p_index[j]] = keys[j];
    }

    keys = holder;

    return pivot_index;

}

// Actual qsort that recursively calls itself with particular partitioning
// strategy to sort the list
void NumList::qsort(vector<int>& keys, int low, int high, ImplType opt)
{
    if(low < high) {
        unsigned int pi;
        if(opt == serial) {
            pi = partition(keys, low, high);
        } else {
            pi = partition_par(keys, low, high);
        }
        qsort(keys, low, pi - 1, opt);
        qsort(keys, pi + 1, high, opt);
    }
}

// wrapper for calling qsort
void NumList::my_qsort(ImplType opt)
{
    /* Initiate the quick sort from this function */
    qsort(list, 0, list.size() - 1, opt);
}
// Default constructor
// This should "create" an empty list
NumList::NumList() {
    /* do nothing */
    /* you will have an empty vector */
}
// Contructor
// Pass in a vector and the partitioning strategy to use
NumList::NumList(vector<int> in, ImplType opt) {
    list = in;
    my_qsort(opt);
}
// Destructor
NumList::~NumList() {
    /* do nothing */
    /* vector will be cleaned up automatically by its destructor */
}
// Get the element at index
int NumList::get_elem(unsigned int index)
{
    return list[index];
}
// Print the list
void NumList::print(ostream& os)
{
    for(unsigned int i = 0; i < list.size(); i++) {
        os << i << ":\t" << list[i] << endl;
    }
}
