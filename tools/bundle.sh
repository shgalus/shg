# bundle:  group files into distribution package

# B. W. Kernighan, R. Pike, The UNIX Programming Environment, 1984,
# page 98.

echo '# Tu unbundle, sh this file'
for i
do
    echo "echo $i 1>&2"
    echo "cat >$i << 'End of $i'"
    cat $i
    echo "End of $i"
done
