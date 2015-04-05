﻿import gfx.io.GameDelegate;

loginButton.addEventListener("click", this, "Login");
function Login()
{
	GameDelegate.call("f2c_Login", [usernameTextInput.text, passwordTextInput.text], this);
}


registerAccountButton.addEventListener("click", this, "registerAccount");
function registerAccount()
{
	GameDelegate.call("f2c_RegisterAccount", [usernameTextInput.text, passwordTextInput.text], this);
}

GameDelegate.addCallBack("c2f_Client_Login", this, "c2f_Client_Login");
function c2f_Client_Login(resultCode:String):Void
{
	if (resultCode=="L2RC_SUCCESS")
	{
		// Save this account
		GameDelegate.call("f2c_SaveProperty", ["DefaultUsername", usernameTextInput.text], this);
		GameDelegate.call("f2c_SaveProperty", ["DefaultPassword", passwordTextInput.text], this);
		
		// Goto logged in screen
		gotoAndStop("Lobby");
	}
	else
	{
		// L2RC_Client_Login_HANDLE_NOT_IN_USE_OR_BAD_SECRET_KEY
		// L2RC_Client_Login_EMAIL_ADDRESS_NOT_VALIDATED
		// L2RC_Client_Login_CDKEY_STOLEN
		// L2RC_Client_Login_BANNED
		
		trace(resultCode);
	}
}

GameDelegate.addCallBack("c2f_Client_RegisterAccount", this, "c2f_Client_RegisterAccount");
function c2f_Client_RegisterAccount(resultCode:String):Void
{
	if (resultCode=="L2RC_SUCCESS")
	{
		// Try to login with this account now
		GameDelegate.call("f2c_Login", [usernameTextInput.text, passwordTextInput.text], this);
	}
	else
	{
		// L2RC_PROFANITY_FILTER_CHECK_FAILED
		// L2RC_Client_RegisterAccount_HANDLE_ALREADY_IN_USE
		// L2RC_Client_RegisterAccount_REQUIRES_CD_KEY
		// L2RC_Client_RegisterAccount_CD_KEY_NOT_USABLE
		// L2RC_Client_RegisterAccount_CD_KEY_NOT_USABLE
		// L2RC_Client_RegisterAccount_CD_KEY_STOLEN
		// L2RC_Client_RegisterAccount_CD_KEY_ALREADY_USED
		
		trace(resultCode);
	}
}

// Ask for last used account
function FillInUsernameAndPassword()
{
	GameDelegate.call("f2c_LoadProperty", ["DefaultUsername"], this, "c2f_LoadProperty_DefaultUsername");
	GameDelegate.call("f2c_LoadProperty", ["DefaultPassword"], this, "c2f_LoadProperty_DefaultPassword");
}


function c2f_LoadProperty_DefaultUsername(property:String):Void
{
	usernameTextInput.text=property;
}

function c2f_LoadProperty_DefaultPassword(property:String):Void
{
	passwordTextInput.text=property;
}

FillInUsernameAndPassword();

stop();
