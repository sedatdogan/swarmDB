const _ = require('lodash');

let sockets = [];
let nodes = [];
let maxNodes = 10;
let minNodes = 8;
let baseAddress = 0;


const Koa = require('koa');

const app = new Koa();

app.use(async ctx => {
    const q = JSON.parse(ctx.request.query.q);
    const specialCommand = specialCommands[q.cmd];
    specialCommand ? specialCommand(q.data) : sendToClients(q.cmd, q.data);
});

app.listen(3003);

const specialCommands = {
    addNodes: (num) => _.times(num, n => sendToClients('updateNodes', [{address: `9999999-${_.uniqueId()}`}]))
};


module.exports = SocketBase => class Socket extends SocketBase {

    websocket(wss) {
        wss.on('connection', (ws, req) => {
            sockets.push(ws);

            ws.on('message', req => {
                req = JSON.parse(req);
                    commandProcessors[req.cmd](req.data)
            });

            ws.on('close', () => {
                _.remove(sockets, ws);
            })
        })
    }
};


const createNodes = (numberOfNodes) => {
    let i = 0;
    while(nodes.length < maxNodes) {
        const node = {
            address: `0x${_.padStart((baseAddress++).toString(16), 2, '0')}`
        };
        nodes.push(node);
        setTimeout(() => {
            sendToClients('updateNodes', [node]);
        },(i++) * 600);
        log(`node ${node.address} has connected`);
    }
};

const sendToClients = (cmd, data) => sockets.forEach(socket => socket.send(JSON.stringify({cmd: cmd, data: data})));

const getRandomNode = () => nodes[Math.floor(Math.random() * nodes.length)];

const updateMessages = () => {
    if(nodes.length) {
        sendToClients('messages', _.times(10, () => (
            {srcAddr: getRandomNode().address, dstAddr: getRandomNode().address, timestamp: new Date().toISOString(), body: {something: `sent - ${_.uniqueId()}`}}
        )));
    }
};

const log = (message) => {
    sendToClients('log', [{
        timer_no: 1,
        entry_no: _.uniqueId(),
        timestamp: new Date().toISOString(),
        message: message
    }]);
};

const killANode = () => {
    if(nodes.length) {
        const node = nodes[Math.floor(Math.random() * nodes.length)];
        _.remove(nodes, node);
        sendToClients('removeNodes', [node.address]);
        log(`node ${node.address} has died`);
    }
};


//setInterval(updateMessages, 1000);
//setInterval(killANode, 6137);
//setInterval(createNodes, 10285);


const commandProcessors = {
    getAllNodes: () => sendToClients('updateNodes', nodes),
    getMaxNodes: () => sendToClients('setMaxNodes', maxNodes),
    getMinNodes: () => sendToClients('setMinNodes', minNodes),
    setMaxNodes: (num) => {
        maxNodes = num;
        sendToClients('setMaxNodes', num);
        log(`Changed max nodes to ${num}`)
    },
    setMinNodes: (num) => {
        minNodes = num;
        sendToClients('setMinNodes', num);
        log(`changed min nodes to ${num}`);
    }
};