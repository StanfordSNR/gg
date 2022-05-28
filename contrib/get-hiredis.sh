cd third_party
git clone --depth 1 -b v1.0.0 https://github.com/redis/hiredis.git
cd hiredis
git apply ../../contrib/hiredis-not-shared.patch

