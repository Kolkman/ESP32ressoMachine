#include "EspressoWebServer.h"
#include <ESPAsyncWebServer.h>
#include <mbedtls/md.h>
#include "debug.h"
#include "pages/WebLogin.html.h"
#include "pages/ESPresso.css.h"
#include "pages/EspressoMachine.svg.h"
#include "pages/switch.css.h"
#include "pages/firmware.js.h"
#include "pages/redCircleCrossed.svg.h"

EspressoWebServer::EspressoWebServer(uint16_t port, const char *username, const char *password) : AsyncWebServer(port)
{
    setUsername(username);
    setPassword(password);
    if (strlen(username) > 0)
    {
        authRequired = true;
    }
    else
    {
        authRequired = false;
    }
}

char *EspressoWebServer::getUsername()
{
    return username;
}

char *EspressoWebServer::getPassword()
{
    return password;
}

void EspressoWebServer::setPassword(const char *p = "")
{
    memset(password, '\0', CREDENTIAL_LENGTH);
    strncpy(password, p, std::max(strlen(p), (size_t)(CREDENTIAL_LENGTH - 1)));
}
void EspressoWebServer::setUsername(const char *u = "")
{
    memset(username, '\0', CREDENTIAL_LENGTH);
    strncpy(username, u, std::max(strlen(u), (size_t)(CREDENTIAL_LENGTH - 1)));
}

void EspressoWebServer::authenticate(AsyncWebServerRequest *request)
{
    LOGINFO1("authenticate for:", String(request->url()));
    if (authRequired)
    {
        if (is_authenticated(request))
        {

            LOGINFO("Authentication Successful");
            return;
        }
        LOGINFO("Authentication Failed");
        AsyncWebServerResponse *response = request->beginResponse(301); // Sends 301 redirect
        response->addHeader("Location", "/WebLogin.html?msg=Authentication Failed, you must log in.");
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
    }
}

// from https://www.mischianti.org/2020/11/09/web-server-with-esp8266-and-esp32-manage-security-and-authentication-4/#Simple_token_authentication
String EspressoWebServer::sha1(String payloadStr)
{
    const char *payload = payloadStr.c_str();

    int size = 20;

    byte shaResult[size];

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;

    const size_t payloadLength = strlen(payload);

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *)payload, payloadLength);
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);

    String hashStr = "";

    for (uint16_t i = 0; i < size; i++)
    {
        String hex = String(shaResult[i], HEX);
        if (hex.length() < 2)
        {
            hex = "0" + hex;
        }
        hashStr += hex;
    }

    return hashStr;
}

// from https://www.mischianti.org/2020/11/09/web-server-with-esp8266-and-esp32-manage-security-and-authentication-4/#Simple_token_authentication
void EspressoWebServer::handleLogin(AsyncWebServerRequest *request)
{
    LOGINFO("Handle login");
    String msg;
    if (request->hasHeader("Cookie"))
    {
        // Print cookies
        String cookie = request->header("Cookie");
        LOGINFO1("Found cookie: ", cookie);
    }
    if (request->hasArg("username") && request->hasArg("password"))
    {
        LOGINFO("Found parameter: ");
        if (request->arg("username") == String(username) && request->arg("password") == String(password))
        {
            AsyncWebServerResponse *response = request->beginResponse(301); // Sends 301 redirect

            String RedirectURL = "/";
            if (request->hasArg("url"))
            {
                RedirectURL = request->arg("url");
            }
            response->addHeader("Location", RedirectURL);
            response->addHeader("Cache-Control", "no-cache");
            String token = sha1(String(username) + ":" + String(password) + ":" + request->client()->remoteIP().toString());
            LOGINFO1("Token: ", token);
            response->addHeader("Set-Cookie", (String)COOKIENAME + "=" + (String)token + "; Max-Age=" + (String)COOKIEMAXAGE);
            request->send(response);
            LOGINFO("Log in Successful");
            return;
        }
        msg = "Wrong username/password! try again.";
        Serial.println("Log in Failed");
        AsyncWebServerResponse *response = request->beginResponse(301); // Sends 301 redirect

        response->addHeader("Location", "/WebLogin.html?msg=" + msg);
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
        return;
    }
}
/**
 * Manage logout (simply remove correct token and redirect to login form)
 */
void EspressoWebServer::handleLogout(AsyncWebServerRequest *request)
{
    Serial.println("Disconnection");
    AsyncWebServerResponse *response = request->beginResponse(301); // Sends 301 redirect
    response->addHeader("Location", "/WebLogin.html?msg=User disconnected");
    response->addHeader("Cache-Control", "no-cache");
    response->addHeader("Set-Cookie", (String)COOKIENAME + "=0");
    request->send(response);
    return;
}
// Check if header is present and correct
bool EspressoWebServer::is_authenticated(AsyncWebServerRequest *request)
{
    LOGINFO("Enter is_authenticated");
    if (request->hasHeader("Cookie"))
    {
        String cookie = request->header("Cookie");
        LOGINFO1("Found cookie: ", cookie);

        String token = sha1(String(username) + ":" +
                            String(password) + ":" +
                            request->client()->remoteIP().toString());
        //  token = sha1(token);
        String ck = (String)COOKIENAME + "=" + token;
        if (cookie.indexOf(ck) != -1)
        {
            LOGINFO("Authentication Successful");
            return true;
        }
        LOGINFO1("Cookiename:", COOKIENAME);
        LOGINFO1("Authentication Failed against:", ck);
    }
    else
    {
        LOGINFO("No cookie in header, Authenticationf failed");
    }
    return false;
}

void EspressoWebServer::handleNotFound(AsyncWebServerRequest *request)
{
    String message = htmlHeader;
    message += "<H1>Error 400 <br/> File Not Found</H1>\n\n";
    message += "<div id=\"notFoundInfo\"><div id=\"notFoundURI\">URI: <span id=\"notFoundURL\">";
    message += request->url();
    message += "</span></div>\n<div id=\"notFoundMethod\">Method: ";
    message += (request->method() == HTTP_GET) ? "GET" : "POST";
    message += "</div>\n<div id=\"notFoundArguments\">Arguments: ";
    message += request->args();
    message += "</div>\n";

    for (uint8_t i = 0; i < request->args(); i++)
    {
        message += "<div class=\"notFoundArgument\"><span class=\"notFoundargName\">" + request->argName(i) + "</span>:<span class=\"notFoundarg\"> " + request->arg(i) + "</span></div>\n";
    }
    message += "</div>";
    message += htmlFooter;
    request->send(404, "text/html", message);
}

void EspressoWebServer::InitPages()
{
    this->onNotFound(std::bind(&EspressoWebServer::handleNotFound, this, std::placeholders::_1));
    this->on("/login", HTTP_POST, std::bind(&EspressoWebServer::handleLogin, this, std::placeholders::_1));
    this->on("/logout", HTTP_GET, std::bind(&EspressoWebServer::handleLogout, this, std::placeholders::_1));

    DEF_HANDLE_WebLogin_html;
    DEF_HANDLE_ESPresso_css;
    DEF_HANDLE_EspressoMachine_svg;
    DEF_HANDLE_redCircleCrossed_svg;
    DEF_HANDLE_ESPresso_css;
    DEF_HANDLE_switch_css;
    DEF_HANDLE_firmware_js;
}

void EspressoWebServer::handleFile(AsyncWebServerRequest *request, const char *mimetype, const unsigned char *compressedData, const size_t compressedDataLen)
{
    AsyncWebServerResponse *response = request->beginResponse(200, mimetype, compressedData, compressedDataLen);
    response->addHeader("Server", "ESP Async Web Server");
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}
