# set cwd to script directory
cd "${0%/*}"

./create_projects.sh

echo building...
cd premake/
make -j4 config=debug_x64

echo done.
exit
