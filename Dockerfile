FROM alpine

# install build dependencies
RUN apk add git make cmake g++ mesa-dev glfw-dev

RUN git clone --single-branch --branch windows https://github.com/numpad/ecs-sandbox.git
WORKDIR ecs-sandbox
RUN cmake .
CMD ["echo", "build done"]
