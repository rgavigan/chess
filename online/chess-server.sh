#!/bin/bash

# Installing Prerequisite Packages
echo "Update and install required packages (-y to skip prompts)"
yum update -y
yum install -y git gcc gcc-c++ boost boost-devel cmake doxygen libpq libpq-devel nginx fcgi libGLEW

# Installing Wt
echo "Download Wt"
cd /home/ec2-user/
wget https://github.com/emweb/wt/archive/4.10.1.tar.gz
tar xvf 4.10.1.tar.gz
cd wt-4.10.1
mkdir build
cd build
cmake ..
echo "Make Wt"
make
echo "Install Wt"
make install
ldconfig
echo "Set Library Path"
echo 'export WT_BASE=/usr/local' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/lib' >> ~./bashrc

# Cloning Git Repo
echo "Clone the git repository"
git clone https://github.com/rgavigan/chess.git /home/ec2-user/chess

# Building Chess Application
echo "Build the application"
make /home/ec2-user/chess

# Creating Nginx Config File
echo "Create an Nginx configuration file"
cat > /etc/nginx/conf.d/chess.conf <<EOL
server {
    server_name chess.rileygavigan.com;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }

    listen 443 ssl;
    ssl_certificate /etc/pki/tls/certs/chess_key.pem;
    ssl_certificate_key /etc/pki/tls/private/chess_key.pem;
    ssl_dhparam /etc/pki/tls/ssl-dhparams.pem;
}

server {
    if ($host = chess.rileygavigan.com) {
        return 301 https://$host$request_uri;
    }
    listen 80;
    server_name chess.rileygavigan.com;
    return 404;
}
EOL

# Creating Self-Signed SSL Certificate - This should be replaced by a real, valid certificate
echo "Create a self-signed certificate"
openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout /etc/pki/tls/private/chess_key.pem -out /etc/pki/tls/certs/chess_key.pem -subj "/C=US/ST=New York/L=New York/O=Chess/OU=Chess/CN=chess.rileygavigan.com"

# Creating DH Parms
echo "Create Diffie-Hellman parameters"
openssl dhparam -out /etc/pki/tls/ssl-dhparams.pem 2048

# Creating systemd service file for chess app
echo "Create a systemd service file"
cat > /etc/systemd/system/chess.rileygavigan.com.service <<EOL
[Unit]
Description=Hosted Chess Application for chess.rileygavigan.com
After=nginx.service
Wants=nginx.service

[Service]
ExecStart=/home/ec2-user/chess main_app --docroot . --http-listen 127.0.0.1:8080
Restart=always
User=ec2-user
WorkingDirectory=/home/ec2-user/chess

[Install]
WantedBy=multi-user.target
EOL

# Starting the application
echo "Start the application"
systemctl daemon-reload
systemctl start chess.rileygavigan.com
systemctl enable chess.rileygavigan.com