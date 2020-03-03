var net=require('net');
var server=net.createServer(function(socket){
	console.log("Client Connected");
        socket.write('Echo server\r\n');
	socket.pipe(socket);
	socket.on('end',function(){
		console.log('Client disconnected');
	});
});
server.listen(8080,function(){
	console.log('Server is listening ');
});
