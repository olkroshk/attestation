# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include ../config.mk

all:
	$(MAKE) genkey
	$(MAKE) build
	$(MAKE) sign

private.pem:
	openssl genrsa -out $@ -3 3072

public.pem: private.pem
	openssl rsa -in $< -out $@ -pubout

genkey: public.pem

build:
	@ echo "Build using: $(SGX_EDGER8R), $(CC), $(CXX)"
	$(SGX_EDGER8R) --trusted enclave.edl --search-path ./ --search-path $(SGX_SDK)/include
	$(CXX) -c $(CXXFLAGS) $(ENCLAVE_CPP_FLAGS) ecalls.cpp crypto.cpp dispatcher.cpp
	$(CC) -c $(CFLAGS) $(ENCLAVE_C_FLAGS) enclave_t.c
	$(CXX) -o genquote_enclave crypto.o dispatcher.o ecalls.o enclave_t.o $(ENCLAVE_LINK_FLAGS)

sign:
	@$(SGX_ENCLAVE_SIGNER) sign -key private.pem -enclave genquote_enclave -out genquote_enclave.debug.signed -config enc.config.xml

##	oesign sign -e genquote_enclave -c enc.conf.release -k private.pem
##	cp genquote_enclave.signed genquote_enclave.release.signed
##	oesign sign -e genquote_enclave -c enc.conf.prodid -k private.pem
##	cp genquote_enclave.signed genquote_enclave.prodid.signed
##	oesign sign -e genquote_enclave -c enc.conf.securityversion -k private.pem
##	cp genquote_enclave.signed genquote_enclave.securityversion.signed

clean:
	rm -f *.o genquote_enclave *.signed enclave_*.* *.pem


