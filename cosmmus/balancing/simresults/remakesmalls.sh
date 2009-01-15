rm small*
g++ avgize.cpp -o avgize
./avgize progrega_kl.data smallpkl.data
./avgize progregakh_kl.data smallpkhkl.data
./avgize progregakf_kl.data smallpkfkl.data
./avgize bfa_kl.data smallbfakl.data

echo << "DONE"

