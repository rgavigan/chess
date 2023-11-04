## Pre-Installation
SSH Into the VM
```sh
ssh <user>@<public-ip-address>
```
Install necessary dependencies for the application
```sh
sudo apt-get install gcc g++ cmake doxygen libboost-all-dev nginx libssl-dev libpq-dev

# Install Wt (can take a long time)
wget -c https://github.com/emweb/wt/archive/4.10.1.tar.gz &&
tar xvxf 4.10.1.tar.gz &&
cd wt-4.10.1 &&
mkdir build && cd build &&
cmake .. && make &&
sudo make install && sudo ldconfig
```

## Building the Application
```sh
cd ~/group2 && make clean && make
```

## Testing the Application Runs
```sh
./main_app --docroot . --http-listen 127.0.0.1:8080

# If the application does not run, try setting the following environmental variables
echo 'export WT_BASE=/usr/local' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/lib' >> ~./bashrc

# Source the file to sync the variables
source ~/.bashrc
```

## Nginx Setup
```sh
# Create the nginx config file
sudo nano /etc/nginx/sites-available/chess.rileygavigan.com
# Fill the file with content from `./config-files/https.chess.com` (or http.chess.rileygavigan.com for just HTTP)

# Enable the nginx config file
sudo ln -s /etc/nginx/sites-available/chess.rileygavigan.com /etc/nginx/sites-enabled/

sudo nginx -t # Test configuration file has no errors

# Start nginx
sudo systemctl start nginx
sudo systemctl status nginx

# Reload nginx if you need to make changes
sudo systemctl reload nginx
```

## Service Setup
```sh
sudo nano /etc/systemd/system/chess.rileygavigan.com.service
# Fill the file with content from `./config-files/chess.rileygavigan.com.service`

sudo systemctl daemon-reload # Reload systemd
sudo systemctl start chess.rileygavigan.com # Start service
sudo systemctl enable chess.rileygavigan.com # Start service at boot time of the VM
sudo systemctl status chess.rileygavigan.com # Verify service is running
sudo ss -tuln | grep 80 # Check that both port 80 & 8080 are running, if so they are both working

# If nginx (port 80) not in the list run
sudo systemctl start nginx
```