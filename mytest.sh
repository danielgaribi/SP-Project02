echo hey > my_output1.txt
echo hey > my_output2.txt
echo hey > my_output3.txt

echo --------------Starting build---------------
python3 setup.py build_ext --inplace
echo -----------Build finished------------------

echo --------------Starting Example 1-----------
time python3 kmeans_pp.py 3 333 test_data/input_1_db_1.txt test_data/input_1_db_2.txt > my_output1.txt
diff -s my_output1.txt test_data/output_1.txt
echo ------------Finished Example 1--------------

echo -----------Starting Example 2---------------
time python3 kmeans_pp.py 7 test_data/input_2_db_1.txt test_data/input_2_db_2.txt > my_output2.txt
diff -s my_output2.txt test_data/output_2.txt
echo -----------Finished Example 2---------------

echo --------------Starting Example 3------------
time python3 kmeans_pp.py 15 750 test_data/input_3_db_1.txt test_data/input_3_db_2.txt > my_output3.txt
diff -s my_output3.txt test_data/output_3.txt
echo --------------Finished Example 3------------

