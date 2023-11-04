server {
    server_name chess.rileygavigan.com;

    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }

    listen 443 ssl;
    ssl_certificate /etc/letsencrypt/live/chess.rileygavigan.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/chess.rileygavigan.com/privkey.pem;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;
}

server {
    if ($host = chess.rileygavigan.com) {
        return 301 https://$host$request_uri;
    }
    listen 80;
    server_name chess.rileygavigan.com;
    return 404;
}