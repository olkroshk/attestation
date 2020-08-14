(WORK IN PROGRESS)

## Samples
* [MAA SGX Attestation Sample Code](https://github.com/gkostal/attestation/tree/master/sgx.attest.sample)
* [MAA SGX Sample Published in Azure Sample Collection](https://github.com/Azure-Samples/microsoft-azure-attestation/tree/main/sgx.attest.sample)
* [Open Enclave SDK Remote Attestation Sample](https://github.com/openenclave/openenclave/tree/master/samples/remote_attestation)

## How to Build and Run

Based on [Greg's Instructions to Build and Run Yourself](https://github.com/gkostal/attestation/tree/master/sgx.attest.sample#instructions-to-build-and-run-yourself) with minor additions. 

### Create an Azure Confidential Computing VM with Ubuntu 18.04
* Log into https://portal.azure.com with personal or corporate account.
  * If you use a corporate account, confirm with the team that they have quota for ACC VM.
  * Find or create a dedicated resource group.
  * Add new resource "Azure Confidential Computing (Virtual Machine)"
    * It is recommended (by Azure) to use SSH keys for remote access to ACC VMs. See [How to use SSH keys with Windows on Azure](https://docs.microsoft.com/en-us/azure/virtual-machines/linux/ssh-from-windows) for more info.
    * Create SSH key: `ssh-keygen -m PEM -t rsa -b 4096`. Select filename and pass-phrase if needed.
    * Copy public key (`cat ~/.ssh/id_rsa.pub` and copy) and paste it into the new VM settings.
    * Use the following command line to connect to the new VM: `ssh -i ~/.ssh/id_rsa.pub azureuser@10.111.12.123`.
### Set-up ACC VM
* Connect to(`ssh -i ~/.ssh/id_rsa.pub azureuser@10.111.12.123`, for example `$ ssh -i ~/.ssh/id_rsa olkroshk@52.151.75.77`) and install prerequisites (source: [Install the Open Enclave SDK (Ubuntu 18.04)](https://github.com/openenclave/openenclave/blob/v0.9.x/docs/GettingStartedDocs/install_oe_sdk-Ubuntu_18.04.md)).
* (Optional) use a terminal multiplexer for work sessions with Linux VMs, such as tmux. I.e run `tmux`.
##### 1. Configure the Intel and Microsoft APT Repositories
```
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu bionic main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -

echo "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-7 main" | sudo tee /etc/apt/sources.list.d/llvm-toolchain-bionic-7.list
wget -qO - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -

echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/18.04/prod bionic main" | sudo tee /etc/apt/sources.list.d/msprod.list
wget -qO - https://packages.microsoft.com/keys/microsoft.asc | sudo apt-key add -
```
##### 2. Install the Intel SGX DCAP Driver
```
sudo apt update
sudo apt -y install dkms
wget https://download.01.org/intel-sgx/sgx-dcap/1.4/linux/distro/ubuntuServer18.04/sgx_linux_x64_driver_1.21.bin -O sgx_linux_x64_driver.bin
chmod +x sgx_linux_x64_driver.bin
sudo ./sgx_linux_x64_driver.bin
```
##### 3. Install the Intel and Open Enclave packages and dependencies
```
sudo apt -y install clang-7 libssl-dev gdb libsgx-enclave-common libsgx-enclave-common-dev libprotobuf10 libsgx-dcap-ql libsgx-dcap-ql-dev az-dcap-client open-enclave=0.9.0
```
Note: as of 2020/08/12 SGX sample requires 0.9.0 version of open-enclave, even though there is 0.10.0 version available. The command above specifies which version of open-enclave needs to be installed (`open-enclave=0.9.0`).  
##### 4. Configure environment variables for Open Enclave SDK for Linux
See [Configure environment variables for Open Enclave SDK for Linux](https://github.com/openenclave/openenclave/blob/v0.9.x/docs/GettingStartedDocs/Linux_using_oe_sdk.md#configure-environment-variables-for-open-enclave-sdk-for-linux) for more details.
```
echo "source /opt/openenclave/share/openenclave/openenclaverc" >> ~/.bashrc
source ~/.bashrc
```
##### 5. (Optional) Stub AZDCAP_DEBUG_LOG_LEVEL
```
export AZDCAP_DEBUG_LOG_LEVEL='NONE'
```
Notice that `NONE` log level is not supported (see ["What is?"](https://github.com/olkroshk/stuff/blob/master/clutter/work-items/ignite-sgx-sample.md#azdcap_debug_log_level) section below), it is used here to stub the environment variable with something other than `INFO`, `WARNING`, or `ERROR`.
If the variable is unset, errors similar to `Azure Quote Provider: libdcap_quoteprov.so [ERROR]: Could not retreive environment variable for 'AZDCAP_DEBUG_LOG_LEVEL'
` are expected later on.

To unset `AZDCAP_DEBUG_LOG_LEVEL` run:
```
unset AZDCAP_DEBUG_LOG_LEVEL
```
### Build and Run 
##### 1. Get conde and build
```
export WORK_DIR=$(pwd)
echo $WORK_DIR
git clone https://github.com/gkostal/attestation.git
cd attestation/sgx.attest.sample/genquotes
make build
```
##### 2. Run Enclave Application
```
make run
```
This runs the application in four different enclave configurations to generate four different remote quotes. 
You should see four new files created in the `$WORK_DIR/attestation/sgx.attest.sample/genquotes/quotes` directory.

Example of expected terminal output:
```
host/genquote_host ./enclave/genquote_enclave.debug.signed              enclave.info.debug.json
    JSON file created: enclave.info.debug.json
host/genquote_host ./enclave/genquote_enclave.release.signed            enclave.info.release.json
    JSON file created: enclave.info.release.json
host/genquote_host ./enclave/genquote_enclave.prodid.signed             enclave.info.prodid.json
    JSON file created: enclave.info.prodid.json
host/genquote_host ./enclave/genquote_enclave.securityversion.signed    enclave.info.securityversion.json
    JSON file created: enclave.info.securityversion.json
```

Example of expected output files:
```
cd $WORK_DIR/attestation/sgx.attest.sample/genquotes/quotes
ls -la ./
-rw-rw-r-- 1 olkroshk olkroshk 10546 Aug 12 21:58 enclave.info.debug.json
-rw-rw-r-- 1 olkroshk olkroshk 10546 Aug 12 21:59 enclave.info.prodid.json
-rw-rw-r-- 1 olkroshk olkroshk 10546 Aug 12 21:59 enclave.info.release.json
-rw-rw-r-- 1 olkroshk olkroshk 10549 Aug 12 21:59 enclave.info.securityversion.json
```
##### 3. Build, run and validate the JSON files with the MAA service
```
cd $WORK_DIR/attestation/sgx.attest.sample/validatequotes.core
./runall.sh
```
'runall.sh' script builds and runs the validation application against the four different JSON files produced earlier.

Example of `runall.sh` terminal output:
```
[22:09:00.413] : ************************************************************************************************************************
[22:09:00.413] : *      VALIDATING MAA JWT TOKEN - MATCHES CLIENT ENCLAVE INFO
[22:09:00.413] : ************************************************************************************************************************
[22:09:00.413] :
[22:09:00.416] : IsDebuggable match                 : True
[22:09:00.416] :     We think   : False
[22:09:00.416] :     MAA service: False
[22:09:00.416] : MRENCLAVE match                    : True
[22:09:00.416] :     We think   : b34abc63333326b626740f0fe66e543eb946847f6b5b4efa19126efe0c20c69b
[22:09:00.416] :     MAA service: b34abc63333326b626740f0fe66e543eb946847f6b5b4efa19126efe0c20c69b
[22:09:00.416] : MRSIGNER match                     : True
[22:09:00.416] :     We think   : 434aa827e97f4b4048cb885d079b153771278494c895c02ae4283897a472e41f
[22:09:00.416] :     MAA service: 434aa827e97f4b4048cb885d079b153771278494c895c02ae4283897a472e41f
[22:09:00.417] : ProductID match                    : True
[22:09:00.417] :     We think   : 1
[22:09:00.417] :     MAA service: 1
[22:09:00.417] : Security Version match             : True
[22:09:00.417] :     We think   : 8888
[22:09:00.417] :     MAA service: 8888
[22:09:00.417] : Enclave Held Data match            : True
[22:09:00.417] :     We think   : LS0tLS1CRUdJTiBQVUJMSUMgS0VZLS0tLS0KTUlJQklqQU5CZ2txaGtpRzl3MEJBUUVGQUFPQ0FROEFNSUlCQ2dLQ0FRRUExSXZY
[22:09:00.417] :                  bEhwQ24xM0F6MWI1YVZVbApGL2dWTExxQUR3eHcwc0pMam5wVVJ5dXBGK0V6MmJYT1cvd1RLOHRwWm5LNWxqQWs4aC9seHY3NUY0
[22:09:00.417] :                  cG9KeG9rCjdOWFFWRU9LZDBwdzhHWVd3QkNLRjVqNk9ERnFCakZJdUY1UWNIQUpQaHF1bEJrNjhTazkxcmJJTDQyd0I4akgKR0pD
[22:09:00.417] :                  dTljbjRJeVJpb2ZFb2NaWkJRdDJKeFlyenNjUWZFSWE4cEluYmxIeXJIbk1BTWJ1aWVHYkVNcHFjcUVGSwpUTmxsaXNyeTFhN0Nu
[22:09:00.417] :                  RjcrSHZUeTJOcDhQd0hkOE9nbmkxNWZON2FRWDd3Qm9yN1dqM0FoNVNWNkJEY0VwK0ttCm9ZNThqWjE4UVU2eCtHUGVFT2I1emdm
[22:09:00.418] :                  MTJGKzhYN0V2Y3IvNDdKR2MydjBSR0lQa24vUUVwUWR0YnZQa2lkOVQKR3dJREFRQUIKLS0tLS1FTkQgUFVCTElDIEtFWS0tLS0t
[22:09:00.418] :                  CgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
[22:09:00.418] :     MAA service: LS0tLS1CRUdJTiBQVUJMSUMgS0VZLS0tLS0KTUlJQklqQU5CZ2txaGtpRzl3MEJBUUVGQUFPQ0FROEFNSUlCQ2dLQ0FRRUExSXZY
[22:09:00.418] :                  bEhwQ24xM0F6MWI1YVZVbApGL2dWTExxQUR3eHcwc0pMam5wVVJ5dXBGK0V6MmJYT1cvd1RLOHRwWm5LNWxqQWs4aC9seHY3NUY0
[22:09:00.418] :                  cG9KeG9rCjdOWFFWRU9LZDBwdzhHWVd3QkNLRjVqNk9ERnFCakZJdUY1UWNIQUpQaHF1bEJrNjhTazkxcmJJTDQyd0I4akgKR0pD
[22:09:00.418] :                  dTljbjRJeVJpb2ZFb2NaWkJRdDJKeFlyenNjUWZFSWE4cEluYmxIeXJIbk1BTWJ1aWVHYkVNcHFjcUVGSwpUTmxsaXNyeTFhN0Nu
[22:09:00.418] :                  RjcrSHZUeTJOcDhQd0hkOE9nbmkxNWZON2FRWDd3Qm9yN1dqM0FoNVNWNkJEY0VwK0ttCm9ZNThqWjE4UVU2eCtHUGVFT2I1emdm
[22:09:00.418] :                  MTJGKzhYN0V2Y3IvNDdKR2MydjBSR0lQa24vUUVwUWR0YnZQa2lkOVQKR3dJREFRQUIKLS0tLS1FTkQgUFVCTElDIEtFWS0tLS0t
[22:09:00.418] :                  CgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
[22:09:00.418] :
[22:09:00.418] :
[22:09:00.418] :
[22:09:00.418] :
```

## Explanation of the process

#### 1. Build genquote executables: genquote_enclave and genquote_host

##### Command
```
make build
```

##### Explanation

* (1) genquotes's Makefile calls the `all` target of [genquotes/enclave/Makefile](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/Makefile#L9)
  * Makefile calls `genkey` target:
    * openssl [creates](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/enclave/Makefile#L16) private key `private.pem`
    * openssl [generates](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/enclave/Makefile#L19) corresponding public key `public.pem`
  * `build` target of the enclave/Makefile is triggered next:
    * oeedger8r tool, available in the Open Enclave SDK, [generates](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/enclave/Makefile#L25) the marshaling code necessary to call functions between the enclave and the host 
    * Make [calls](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/enclave/Makefile#L28) c++ compiler to build executable `genquote_enclave` 
  * Next step is the [`sign` target](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/enclave/Makefile#L30):
    * `oesign` tool [is called 4 times](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/enclave/Makefile#L31-L38) for each configuration file located in genquotes/enclave, followed by corresponding renaming of the output. The `oesign` tool signs an enclave image file with provided RSA private key in PEM format. The enclave's properties are read from the passed configuration file. These properties override any properties that were already defined inside the enclave image 
* (2) genquotes/host/Makefile is [called](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/host/Makefile#L10)
  * oeedger8r generates marshaling code for the untrusted part of the enclave specified in `remoteattestation.edl`
  * C++ compiler builds the `genquote_host` executable

##### Output files

* 2 executables:
```
	host/genquote_host
	enclave/genquote_enclave
```
* 4 signed enclave images:
```
	enclave/genquote_enclave.debug.signed              
	enclave/genquote_enclave.release.signed           
	enclave/genquote_enclave.prodid.signed             
	enclave/genquote_enclave.securityversion.signed   
```

#### 2. Run genquote_host for each of signed enclave images

##### Command
```
make run
```

##### Explanation

The [`ran` target](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/Makefile#L16) calls `genquote_host` 4 times (once per each signed enclave image). `genquote_host` calls into the enclave (via an ecall) to retrieve a remote quote, parses the remote quote to retrieve important attributes, and persists the remote quote, enclave held data and parsed report fields to a JSON file on disk. See https://github.com/gkostal/attestation/tree/master/sgx.attest.sample#overview for more details.

##### Output files

* 4 JSON files, one per each signed enclave image:
```
	quotes/enclave.info.debug.json
	quotes/enclave.info.release.json
	quotes/enclave.info.prodid.json
	quotes/enclave.info.securityversion.json
```

The [`ran` target](https://github.com/gkostal/attestation/blob/master/sgx.attest.sample/genquotes/Makefile#L16) calls `genquote_host` application to 

## Troubleshooting

(WRITE ME)

## What is?

##### AZDCAP_DEBUG_LOG_LEVEL 
`AZDCAP_DEBUG_LOG_LEVEL` is used to enable logging to stdout for debug purposes. Supported values are `INFO`, `WARNING`, and `ERROR`; any other values will fail silently. If a logging callback is set by the caller such as open enclave this setting will be ignored as the logging callback will have precedence. Log levels follow standard behavior: INFO logs everything, WARNING logs warnings and errors, and ERROR logs only errors. Default setting has logging off. These capatalized values are represented internally as strings.

Source: [Azure DCAP Client's configuration](https://github.com/microsoft/Azure-DCAP-Client#configuration)

##### Enclave Definition Language (EDL)
An enclave’s bridge functions are prototyped in EDL files.

When the project is built, the Edger8r tool that is included with the Intel SGX SDK parses the EDL file and generates a series of proxy functions. These proxy functions are essentially wrappers around the real functions that are prototyped in the EDL [SGX101](https://sgx101.gitbook.io/sgx101/sgx-bootstrap/enclave).

##### ECALL
 “Enclave Call”, a call made into an interface function within the enclave [SGX101](https://sgx101.gitbook.io/sgx101/sgx-bootstrap/enclave).

(WRITE ME)

## References

##### [1] [Greg's MAA SGX Attestation Sample Code](https://github.com/gkostal/attestation/tree/master/sgx.attest.sample)
##### [2] [Greg's Instructions to Build and Run Yourself](https://github.com/gkostal/attestation/tree/master/sgx.attest.sample#instructions-to-build-and-run-yourself )
##### [3] [Azure confidential computing](https://azure.microsoft.com/en-us/solutions/confidential-compute/)
##### [4] [Install the Open Enclave SDK (Ubuntu 18.04)](https://github.com/openenclave/openenclave/blob/v0.9.x/docs/GettingStartedDocs/install_oe_sdk-Ubuntu_18.04.md)
##### [5] [Install .NET Core SDK or .NET Core Runtime on Ubuntu](https://docs.microsoft.com/en-us/dotnet/core/install/linux-ubuntu)
##### [6] [Configure environment variables for Open Enclave SDK for Linux](https://github.com/openenclave/openenclave/blob/v0.9.x/docs/GettingStartedDocs/Linux_using_oe_sdk.md#configure-environment-variables-for-open-enclave-sdk-for-linux)
##### [7] [How to use SSH keys with Windows on Azure](https://docs.microsoft.com/en-us/azure/virtual-machines/linux/ssh-from-windows)
##### [8] [Azure DCAP Client](https://github.com/microsoft/Azure-DCAP-Client)


