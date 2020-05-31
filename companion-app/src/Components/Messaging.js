import React, { Component } from 'react';
import { API, graphqlOperation, Auth } from 'aws-amplify';
import { Connect } from 'aws-amplify-react';
import * as queries from '../graphql/queries';
import * as subscriptions from '../graphql/subscriptions';
import * as mutations from '../graphql/mutations';

export default class Messaging extends Component {
    constructor(props) {
        super(props);

        this.state = {
            messageToSend: "",
            receivedMessages: [
       //         { owner: "", contents: "" }
            ],
            users: [{ name: "" }],
            showingMessages: false
        }
    }

    updateMessage = (event) => {
        this.setState({
            messageToSend: event.target.value
        });
    }

    sendMessage = async () => {
        let user = await Auth.currentAuthenticatedUser();
        console.log(this.state.users);

        const messageDetails = {
            owner: user.username,
            contents: this.state.messageToSend,
            recipients: this.state.users
        };

        const storedMessage = await API.graphql(graphqlOperation(mutations.createMessage, {input: messageDetails}));
        console.log(storedMessage);
    }

    updateUsersList = idx => event => {
        const newUsers = this.state.users.map((user, sidx) => {
          if(idx !== sidx) {
            return user;
          }

          return { ...user, name: event.target.value };
        })

        this.setState({ users: newUsers })
    }

    removeUser = idx => () => {
        this.setState({
            users: this.state.users.filter((s, sidx) => idx !== sidx)
        });
    }

    addUser = () => {
        this.setState({
            users: this.state.users.concat([{ name: "" }])
        });
    }

    // this should be temporary until we get real time display working
    getMessages = async () => {
        let receivedMessages = [];
        if(!this.state.showingMessages) {
            receivedMessages = this.state.receivedMessages.concat();
            let user = await Auth.currentAuthenticatedUser();
            const currentUser = user.username;

            const allMessages = await API.graphql(graphqlOperation(queries.listMessages));
            const allMessageDetails = allMessages.data.listMessages.items;
            
            //console.log(allMessages);
            for (const i in allMessageDetails) {
                //console.log(allMessageDetails[i].owner);
                if(allMessageDetails[i].owner === currentUser) {
                    receivedMessages.push({
                        owner: currentUser,
                        contents: allMessageDetails[i].contents
                    });
                }

                const recipients = allMessageDetails[i].recipients;
                for (const j in recipients) {
                    const recipientUserName = recipients[j].substring(6, recipients[j].length - 1);
                    const messageOwner = allMessageDetails[j].owner;
                    if(recipientUserName === currentUser) {
                        receivedMessages.push({
                            owner: messageOwner,
                            contents: allMessageDetails[i].contents
                        });
                    }
                }
            }
        }

        console.log(receivedMessages);

        let messageState = !this.state.showingMessages;
        this.setState({ receivedMessages: receivedMessages, showingMessages: messageState });
    }

    render() {
        const MessageList = ({ messages }) => (
            <ul>
                {messages.map(message => <li key={message.id}> ({message.owner}) {message.contents} </li>)}
            </ul>
        );
        return (
            <div>
                <form>
                  <h3> Send to </h3>
                    { this.state.users.map((user, idx) => (
                      <div>
                        <input 
                          type="text"
                          placeholder={`User #${idx + 1}`}
                          value={user.name}
                          onChange={this.updateUsersList(idx)}
                        />
                        <button
                          type="button"
                          onClick={this.removeUser(idx)}
                        >
                          Remove
                        </button>
                      </div>
                    )) }
                    <button
                      type="button"
                      onClick={this.addUser}
                    >Add User</button>
                </form>
                <p></p>
                <input type="text" placeholder="Message..." onChange={this.updateMessage} />
                <button onClick={this.sendMessage}>Send</button>
                <h3>Current Group Messages</h3>
                <p>TODO: Have this update in real time</p>
                <button onClick={this.getMessages}>Show/Hide Messages</button>
                <ul>
                    { this.state.receivedMessages.map(message => (
                        <li> { message.owner }: {message.contents} </li>
                    )) }
                </ul>
                {/*
                <Connect 
                    query={graphqlOperation(queries.listMessages)}
                    subscription={graphqlOperation(subscriptions.onCreateMessage)}
                    onSubscriptionMsg={(prev, { onCreateMessage }) => {
                        console.log(onCreateMessage);
                        return prev;
                    }}
                >
                    {({ data: {listMessages}, loading, errors }) => {
                        if(errors) {
                            return (<h3>Error</h3>);
                        }

                        if(loading || !listMessages) {
                            return (<h3>Loading...</h3>);
                        }

                        return (<MessageList messages={listMessages ? listMessages.items :  []} />);
                    }}
                </Connect>
                */}
            </div>
        );
    }
}