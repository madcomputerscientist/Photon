export default (request) => {
    const xhr = require("xhr");

    const settings = {
        username: "alexander.yao@outlook.com",
        password: "Open1234",
        event: "SMARTLAMP"
    };
    
    const authUrl = "https://api.particle.io/oauth/token";
    const authOptions = {
        "method": "POST",
        "headers": {
            "Content-Type": "application/x-www-form-urlencoded",
            "Authorization": "Basic cGFydGljbGU6cGFydGljbGU="
        },
        "body": "grant_type=password&username=" + settings.username + "&password=" + settings.password
    };

    console.log(authOptions);
    
    return xhr.fetch(authUrl, authOptions).then((response) => {
        const body = JSON.parse(response.body);
        console.log(body);
        console.log(request);
        
        const publishUrl = "https://api.particle.io/v1/devices/events";
        const publishOptions = {
            "method": "POST",
            "headers": {
                "Content-Type": "application/x-www-form-urlencoded",
            },
            "body": "name=" + settings.event + "&data=" + request.message.command + "&private=false&ttl=60&access_token=" + body.access_token
        };
        console.log(publishOptions);
        
        return xhr.fetch(publishUrl, publishOptions).then((response) => {
            console.log(response.body);
            
            return request.ok();
        });
    });
}