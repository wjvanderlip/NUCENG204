# Tests for the trivial example code in code/example.py


from location_data import add

from nose.tools import assert_raises


#need to call functions into this thing and make random tests
def test_add_works():
    assert add(2, 2) == 4

def test_add_commutative():
    assert add(1, 3) == add(3, 1)

def test_add_bad_inputs():
    a = 1
    b = "I'm a string"
    assert_raises(TypeError, add, a, b)
