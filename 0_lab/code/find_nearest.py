import numpy as np

def find_near(array,value):

    """
    insert an array you're looking for a value near
    and the value you're looking for
    outputs teh index of that array value as well
    as the value itself
    """
    diff=np.abs(np.array(array)-value)
    sort_diff=sorted(diff)
    diff=diff.tolist()
    index=diff.index(sort_diff[0])
    return index, array[index]
