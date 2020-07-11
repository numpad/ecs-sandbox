FROM archlinux

# install build dependencies
RUN pacman -Syu --noconfirm
RUN pacman -S --noconfirm git make cmake gcc mesa

# RUN git clone --single-branch --branch windows https://github.com/numpad/ecs-sandbox.git
COPY . ecs-sandbox/
WORKDIR ecs-sandbox
RUN ls lib/glfw/
RUN cmake .
CMD ["echo", "build done"]
