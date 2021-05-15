FROM node:14.5.0-alpine3.11

# Create app directory
WORKDIR /opt/src/app

# Install app dependencies
# A wildcard is used to ensure both package.json AND package-lock.json are copied
# where available (npm@5+)
COPY package*.json ./

RUN npm install

# Bundle app source
COPY . .

# Listen http server port 
EXPOSE 9000
ENTRYPOINT [ "node", "server.js" ]
