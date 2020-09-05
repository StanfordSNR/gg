check: examples/add_str
	python3 -m unittest

examples/add_str: examples/add_str.cc
	c++ -o $@ $<

check_remote: examples/add_str
	REMOTE_TEST=1 python3 -m unittest

type:
	(cd .. && mypy pygg) && mypy examples/*.py

clean:
	rm -f examples/add_str
