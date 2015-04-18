#!/usr/bin/python

from Agent import Agent

if __name__ == '__main__':

    # Init agent and read config
    agent = Agent()
    agent.parseConfig()

    # connect to server
    if agent.connect():
        agent.monitor()
        agent.append_plugins()
        agent.parser()

