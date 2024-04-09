## Online Chess Application Setup Tutorial / Instructions

### Infrastructure
* AWS provides AWS CDK - it makes deploying EC2 instances very easy, and a script has been created to run the application as a service upon the EC2 instance's creation.

#### Azure Setup
* **OS**: Ubuntu 20.04
* **Type**: Standard D2as v4 (2 vCPUs, 8GiB RAM)
* **Storage**: 30GB root volume
* ****Networking**: Allows any SSH/HTTP/HTTPS inbound traffic

### Managing AWS Compute Resources With CDK
To improve automation of managing this instance and related resources (VPC, Subnets, Security Group, etc.), AWS CDK is used with TypeScript.

[The EC2 Instance is created here](https://github.com/rgavigan/portfolio-cdk/blob/main/lib/ec2-stack.ts). For instructions on how to run and deploy the CDK, view the [README](https://github.com/rgavigan/portfolio-cdk/blob/main/README.md).
* **OS**: Amazon Linux 2023
* **Type**: t2.micro
* **Storage**: 30GB root volume
* **Networking**: Security Group Allows any SSH/HTTP/HTTPS inbound traffic

### Initialization of the EC2 Instance
[This bash script](https://github.com/rgavigan/portfolio-cdk/blob/main/scripts/chess-server.sh) (alternatively, navigate to `./chess-server.sh`) is ran whenever the EC2 Instance is deployed. 
* Handles installing dependencies (i.e. gcc, Wt, Nginx)
* Clones the Chess repository (GitHub source) and builds the application
* Configures Nginx for the application
* Creates a self-signed SSL certificate (to be replaced) and runs the application as a service on the EC2 instance

### More Information
* Navigate to `./AZURE_APP_SETUP.md` to view detailed steps on how to deploy the application through an Azure VM after having created one.
