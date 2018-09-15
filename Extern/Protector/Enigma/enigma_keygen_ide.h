 // Key generation/verification results
const int  EP_NO_ERROR                      = 0;
const int  EP_ERROR_UNKNOWN                 = 1;
const int  EP_ERROR_KEYBUFFEREMPTY          = 2;
const int  EP_ERROR_KEYBUFFERISLESS         = 3;
const int  EP_ERROR_REGINFOEMPTY            = 4;
const int  EP_ERROR_REGINFOTOOLARGE         = 5;
const int  EP_ERROR_PRIVATEKEYISNOTSET      = 6;
const int  EP_ERROR_PUBLICKEYISNOTSET       = 7;
const int  EP_ERROR_PRIVATEKEYISINVALID     = 8;
const int  EP_ERROR_PUBLICKEYISINVALID      = 9;
const int  EP_ERROR_KEYMODEISINVALID        = 10;
const int  EP_ERROR_KEYBASEISINVALID        = 11;
const int  EP_ERROR_CURRENTDATEISINVALID    = 12;
const int  EP_ERROR_EXPIRATIONDATEISINVALID = 13;
const int  EP_ERROR_KEYISINVALID            = 14;
const int  EP_ERROR_HARDWAREID              = 15;
const int  EP_ERROR_HARDWAREBUFFEREMPTY     = 16;
const int  EP_ERROR_HARDWAREIDINVALIDFORKEY = 17;
const int  EP_ERROR_PROJECTFILENOTFOUND     = 18;
const int  EP_ERROR_INVALIDPROJECTFILE      = 19;
const int  EP_ERROR_EXECUTIONSNUMBERINVALID = 20;
const int  EP_ERROR_DAYSNUMBERINVALID       = 21;
const int  EP_ERROR_COUNTRYCODEINVALID      = 22;
const int  EP_ERROR_RUNTIMEINVALID          = 23;
const int  EP_ERROR_GLOBALTIMEINVALID       = 24;
const int  EP_ERROR_INSTALLBEFOREINVALID    = 25;
const int  EP_ERROR_INSTALLAFTERINVALID     = 26;

  // Number of crypted sections
const int  NUMBER_OF_CRYPTED_SECTIONS       = 16;

  // Registartion mode types
const int  RM_512  = 0;
const int  RM_768  = 1;
const int  RM_1024 = 2;
const int  RM_2048 = 3;
const int  RM_3072 = 4;
const int  RM_4096 = 5;

  // Registration Base types
const int  RB_2  = 0;
const int  RB_8  = 1;
const int  RB_16 = 2;
const int  RB_32 = 3;
const int  RB_64 = 4;

  // Counties code
const int CN_AFGHANISTAN = 114;
const int CN_ALBANIA = 1;
const int CN_ALGERIA = 2;
const int CN_ARGENTINA = 3;
const int CN_ARMENIA = 4;
const int CN_AUSTRALIA = 5;
const int CN_AUSTRIA = 6;
const int CN_AZERBAIJAN = 7;
const int CN_BAHRAIN = 8;
const int CN_BANGLADESH = 115;
const int CN_BELARUS = 9;
const int CN_BELGIUM = 10;
const int CN_BELIZE = 11;
const int CN_BOLIVIA = 116;
const int CN_BOSNIA = 117;
const int CN_BRAZIL = 13;
const int CN_BRUNEI = 14;
const int CN_BULGARIA = 15;
const int CN_CAMBODIA = 16;
const int CN_CANADA = 17;
const int CN_CARRIBEAN = 118;
const int CN_CHILE = 20;
const int CN_CHINA = 21;
const int CN_COLOMBIA = 22;
const int CN_COSTARICA = 23;
const int CN_CROATIA = 24;
const int CN_CZECH = 25;
const int CN_DENMARK = 26;
const int CN_DOMINICAN = 27;
const int CN_ECUADOR = 28;
const int CN_EGYPT = 29;
const int CN_ELSALVADOR = 30;
const int CN_ESTONIA = 31;
const int CN_ETHIOPIA = 119;
const int CN_FAROE = 32;
const int CN_FINLAND = 33;
const int CN_FRANCE = 34;
const int CN_GEORGIA = 35;
const int CN_GERMANY = 36;
const int CN_GREECE = 37;
const int CN_GREENLAND = 120;
const int CN_GUATEMALA = 38;
const int CN_HONDURAS = 39;
const int CN_HONGKONG = 40;
const int CN_HUNGARU = 41;
const int CN_ICELAND = 42;
const int CN_INDIA = 43;
const int CN_INDONESIA = 44;
const int CN_IRAN = 45;
const int CN_IRAQ = 46;
const int CN_IRELAND = 47;
const int CN_ISRAEL = 48;
const int CN_ITALY = 49;
const int CN_JAMAICA = 50;
const int CN_JAPAN = 51;
const int CN_JORDAN = 52;
const int CN_KAZAKHSTAN = 53;
const int CN_KENYA = 54;
const int CN_KOREA = 56;
const int CN_KUWAIT = 57;
const int CN_KYRGYZSTAN = 58;
const int CN_LAOS = 121;
const int CN_LATVIA = 59;
const int CN_LEBANON = 60;
const int CN_LIBYAN = 122;
const int CN_LIECHTENSTEIN = 62;
const int CN_LITHUANIA = 63;
const int CN_LUXEMBOURG = 64;
const int CN_MACAO = 65;
const int CN_MACEDONIA = 66;
const int CN_MALAYSIA = 67;
const int CN_MALDIVES = 123;
const int CN_MALTA = 124;
const int CN_MEXOCI = 68;
const int CN_MONACO = 70;
const int CN_MONGOLIA = 71;
const int CN_MONTENEGRO = 125;
const int CN_MOROCCO = 72;
const int CN_NEPAL = 126;
const int CN_NETHERLANDS = 73;
const int CN_NEWZEALAND = 74;
const int CN_NICARAGUA = 75;
const int CN_NIGERIA = 127;
const int CN_NORWAY = 76;
const int CN_OMAN = 77;
const int CN_PAKISTAN = 78;
const int CN_PANAMA = 79;
const int CN_PARAGUAY = 80;
const int CN_PERY = 81;
const int CN_PHILIPPINES = 82;
const int CN_POLAND = 83;
const int CN_PORTUGAL = 84;
const int CN_PUERTORICO = 85;
const int CN_QATAR = 86;
const int CN_ROMANIA = 87;
const int CN_RUSSIA = 88;
const int CN_RWANDA = 128;
const int CN_SAUDIARABIA = 89;
const int CN_SENEGAL = 129;
const int CN_SERBIA = 130;
const int CN_SERBIAMONTENEGRO = 90;
const int CN_SINGAROPE = 91;
const int CN_SLOVAKIA = 92;
const int CN_SLOVENIA = 93;
const int CN_SOUTHAFRICA = 94;
const int CN_SPAIN = 95;
const int CN_SRILANKA = 131;
const int CN_SWEDEN = 96;
const int CN_SWITZERLAND = 97;
const int CN_SYRIAN = 132;
const int CN_TAIWAN = 98;
const int CN_TAJIKISTAN = 99;
const int CN_THAILAND = 100;
const int CN_TRINIDADTOBAGO = 101;
const int CN_TUNISIA = 102;
const int CN_TURKEY = 103;
const int CN_TURKMENISTAN = 133;
const int CN_UKRAINE = 104;
const int CN_UAE = 105;
const int CN_UNITEDKINGDOM = 106;
const int CN_USA = 107;
const int CN_URUGUAY = 108;
const int CN_UZBEKISTAN = 109;
const int CN_VENEZUELA = 110;
const int CN_VIETNAM = 111;
const int CN_YEMEN = 112;
const int CN_ZIMBABWE = 113;

typedef struct _TKeyCountries
{
  char* Name;
  int Code;
} TKeyCountries, *PKeyCountries;

const TKeyCountries KEY_COUNTRIES[127] =
{
  "Afghanistan"               , CN_AFGHANISTAN      ,
  "Albania"                   , CN_ALBANIA          ,
  "Algeria"                   , CN_ALGERIA          ,
  "Argentina"                 , CN_ARGENTINA        ,
  "Armenia"                   , CN_ARMENIA          ,
  "Australia"                 , CN_AUSTRALIA        ,
  "Austria"                   , CN_AUSTRIA          ,
  "Azerbaijan"                , CN_AZERBAIJAN       ,
  "Bahrain"                   , CN_BAHRAIN          ,
  "Bangladesh"                , CN_BANGLADESH       ,
  "Belarus"                   , CN_BELARUS          ,
  "Belgium"                   , CN_BELGIUM          ,
  "Belize"                    , CN_BELIZE           ,
  "Bolivia"                   , CN_BOLIVIA          ,
  "Bosnia and Herzegovina"    , CN_BOSNIA           ,
  "Brazil"                    , CN_BRAZIL           ,
  "Brunei Darussalam"         , CN_BRUNEI           ,
  "Bulgaria"                  , CN_BULGARIA         ,
  "Cambodia"                  , CN_CAMBODIA         ,
  "Canada"                    , CN_CANADA           ,
  "Caribbean"                 , CN_CARRIBEAN        ,
  "Chile"                     , CN_CHILE            ,
  "China"                     , CN_CHINA            ,
  "Colombia"                  , CN_COLOMBIA         ,
  "Costa Rica"                , CN_COSTARICA        ,
  "Croatia"                   , CN_CROATIA          ,
  "Czech Republic"            , CN_CZECH            ,
  "Denmark"                   , CN_DENMARK          ,
  "Dominican Republic"        , CN_DOMINICAN        ,
  "Ecuador"                   , CN_ECUADOR          ,
  "Egypt"                     , CN_EGYPT            ,
  "El Salvador"               , CN_ELSALVADOR       ,
  "Estonia"                   , CN_ESTONIA          ,
  "Ethiopia"                  , CN_ETHIOPIA         ,
  "Faroe Islands"             , CN_FAROE            ,
  "Finland"                   , CN_FINLAND          ,
  "France"                    , CN_FRANCE           ,
  "Georgia"                   , CN_GEORGIA          ,
  "Germany"                   , CN_GERMANY          ,
  "Greece"                    , CN_GREECE           ,
  "Greenland"                 , CN_GREENLAND        ,
  "Guatemala"                 , CN_GUATEMALA        ,
  "Honduras"                  , CN_HONDURAS         ,
  "Hong Kong"                 , CN_HONGKONG         ,
  "Hungary"                   , CN_HUNGARU          ,
  "Iceland"                   , CN_ICELAND          ,
  "India"                     , CN_INDIA            ,
  "Indonesia"                 , CN_INDONESIA        ,
  "Iran"                      , CN_IRAN             ,
  "Iraq"                      , CN_IRAQ             ,
  "Ireland"                   , CN_IRELAND          ,
  "Israel"                    , CN_ISRAEL           ,
  "Italy"                     , CN_ITALY            ,
  "Jamaica"                   , CN_JAMAICA          ,
  "Japan"                     , CN_JAPAN            ,
  "Jordan"                    , CN_JORDAN           ,
  "Kazakhstan"                , CN_KAZAKHSTAN       ,
  "Kenya"                     , CN_KENYA            ,
  "Korea"                     , CN_KOREA            ,
  "Kuwait"                    , CN_KUWAIT           ,
  "Kyrgyzstan"                , CN_KYRGYZSTAN       ,
  "Laos"                      , CN_LAOS             ,
  "Latvia"                    , CN_LATVIA           ,
  "Lebanon"                   , CN_LEBANON          ,
  "Libyan"                    , CN_LIBYAN           ,
  "Liechtenstein"             , CN_LIECHTENSTEIN    ,
  "Lithuania"                 , CN_LITHUANIA        ,
  "Luxembourg"                , CN_LUXEMBOURG       ,
  "Macao"                     , CN_MACAO            ,
  "Macedonia"                 , CN_MACEDONIA        ,
  "Malaysia"                  , CN_MALAYSIA         ,
  "Maldives"                  , CN_MALDIVES         ,
  "Malta"                     , CN_MALTA            ,
  "Mexico"                    , CN_MEXOCI           ,
  "Monaco"                    , CN_MONACO           ,
  "Mongolia"                  , CN_MONGOLIA         ,
  "Montenegro"                , CN_MONTENEGRO       ,
  "Morocco"                   , CN_MOROCCO          ,
  "Nepal"                     , CN_NEPAL            ,
  "Netherlands"               , CN_NETHERLANDS      ,
  "New Zealand"               , CN_NEWZEALAND       ,
  "Nicaragua"                 , CN_NICARAGUA        ,
  "Nigeria"                   , CN_NIGERIA          ,
  "Norway"                    , CN_NORWAY           ,
  "Oman"                      , CN_OMAN             ,
  "Pakistan"                  , CN_PAKISTAN         ,
  "Panama"                    , CN_PANAMA           ,
  "Paraguay"                  , CN_PARAGUAY         ,
  "Peru"                      , CN_PERY             ,
  "Philippines"               , CN_PHILIPPINES      ,
  "Poland"                    , CN_POLAND           ,
  "Portugal"                  , CN_PORTUGAL         ,
  "Puerto Rico"               , CN_PUERTORICO       ,
  "Qatar"                     , CN_QATAR            ,
  "Romania"                   , CN_ROMANIA          ,
  "Russia"                    , CN_RUSSIA           ,
  "Rwanda"                    , CN_RWANDA           ,
  "Saudi Arabia"              , CN_SAUDIARABIA      ,
  "Senegal"                   , CN_SENEGAL          ,
  "Serbia"                    , CN_SERBIA           ,
  "Serbia and Montenegro"     , CN_SERBIAMONTENEGRO ,
  "Singapore"                 , CN_SINGAROPE        ,
  "Slovakia"                  , CN_SLOVAKIA         ,
  "Slovenia"                  , CN_SLOVENIA         ,
  "South Africa"              , CN_SOUTHAFRICA      ,
  "Spain"                     , CN_SPAIN            ,
  "Sri Lanka"                 , CN_SRILANKA         ,
  "Sweden"                    , CN_SWEDEN           ,
  "Switzerland"               , CN_SWITZERLAND      ,
  "Syrian"                    , CN_SYRIAN           ,
  "Taiwan"                    , CN_TAIWAN           ,
  "Tajikistan"                , CN_TAJIKISTAN       ,
  "Thailand"                  , CN_THAILAND         ,
  "Trinidad and Tobago"       , CN_TRINIDADTOBAGO   ,
  "Tunisia"                   , CN_TUNISIA          ,
  "Turkey"                    , CN_TURKEY           ,
  "Turkmenistan"              , CN_TURKMENISTAN     ,
  "Ukraine"                   , CN_UKRAINE          ,
  "United Arab Emirates"      , CN_UAE              ,
  "United Kingdom"            , CN_UNITEDKINGDOM    ,
  "United States"             , CN_USA              ,
  "Uruguay"                   , CN_URUGUAY          ,
  "Uzbekistan"                , CN_UZBEKISTAN       ,
  "Venezuela"                 , CN_VENEZUELA        ,
  "Viet Nam"                  , CN_VIETNAM          ,
  "Yemen"                     , CN_YEMEN            ,
  "Zimbabwe"                  , CN_ZIMBABWE
};

typedef struct _TKeyVerifyParams
{
    char* RegInfo;             // {in} registration info/name
    DWORD RegInfoLen;          // {in} registration info/name size
    DWORD KeyMode;             // {in} key generation mode (~RSA ???)
    DWORD KeyBase;             // {in} key output base (Base ???)
    char* Key;                 // {in} buffer for registration key
    DWORD KeyLen;              // {in} registration key size
    DWORD CreationYear;        // {out} key creation year
    DWORD CreationMonth;       // {out} key creation month
    DWORD CreationDay;         // {out} key creation day
    bool UseKeyExpiration;     // {out} has key expiration date?
    DWORD ExpirationYear;      // {out} key expiration year
    DWORD ExpirationMonth;     // {out} key expiration month
    DWORD ExpirationDay;       // {out} key expiration day
    bool UseHardwareLocking;   // {in} hardware locked key
    char* HardwareID;          // {in} pointer to null terminated hardware string
    bool UseExecutionsLimit;   // {out} limit key by executions?
    DWORD ExecutionsCount;     // {out} number of executions
    bool UseDaysLimit;         // {out} limit key by days?
    DWORD DaysCount;           // {out} number of days
    bool UseRunTimeLimit;      // {out} limit key by run time?
    DWORD RunTimeMinutes;      // {out} run time minutes
    bool UseGlobalTimeLimit;   // {out} limit key by global time?
    DWORD GlobalTimeMinutes;   // {out} global time minutes
    bool UseCountyLimit;       // {out} limit key by country?
    DWORD CountryCode;         // {out} country code
    bool UseRegisterAfter;     // {out} register key after date?
    DWORD RegisterAfterYear;   // {out} register after year
    DWORD RegisterAfterMonth;  // {out} register after month
    DWORD RegisterAfterDay;    // {out} register after day
    bool UseRegisterBefore;    // {out} register key before date?
    DWORD RegisterBeforeYear;  // {out} register before year
    DWORD RegisterBeforeMonth; // {out} register before month
    DWORD RegisterBeforeDay;   // {out} register before day
    DWORD EncryptedConstant;   // {in} Encryption Constant - should be extracted from project file
    bool EncryptedSections[NUMBER_OF_CRYPTED_SECTIONS]; // {out} Crypted sections
    char* PublicKey;           // {in} Public Key - should be extracted from project file
} TKeyVerifyParams, *PKeyVerifyParams;

typedef struct _TKeyVerifyParamsA
{
    char* RegInfo;             // {in} registration info/name
    DWORD KeyMode;             // {in} key generation mode (~RSA ???)
    DWORD KeyBase;             // {in} key output base (Base ???)
    char* Key;                 // {in} buffer for registration key
    DWORD KeyLen;              // {in} registration key size
    DWORD CreationYear;        // {out} key creation year
    DWORD CreationMonth;       // {out} key creation month
    DWORD CreationDay;         // {out} key creation day
    bool UseKeyExpiration;     // {out} has key expiration date?
    DWORD ExpirationYear;      // {out} key expiration year
    DWORD ExpirationMonth;     // {out} key expiration month
    DWORD ExpirationDay;       // {out} key expiration day
    bool UseHardwareLocking;   // {in} hardware locked key
    char* HardwareID;          // {in} pointer to null terminated hardware string
    bool UseExecutionsLimit;   // {out} limit key by executions?
    DWORD ExecutionsCount;     // {out} number of executions
    bool UseDaysLimit;         // {out} limit key by days?
    DWORD DaysCount;           // {out} number of days
    bool UseRunTimeLimit;      // {out} limit key by run time?
    DWORD RunTimeMinutes;      // {out} run time minutes
    bool UseGlobalTimeLimit;   // {out} limit key by global time?
    DWORD GlobalTimeMinutes;   // {out} global time minutes
    bool UseCountyLimit;       // {out} limit key by country?
    DWORD CountryCode;         // {out} country code
    bool UseRegisterAfter;     // {out} register key after date?
    DWORD RegisterAfterYear;   // {out} register after year
    DWORD RegisterAfterMonth;  // {out} register after month
    DWORD RegisterAfterDay;    // {out} register after day
    bool UseRegisterBefore;    // {out} register key before date?
    DWORD RegisterBeforeYear;  // {out} register before year
    DWORD RegisterBeforeMonth; // {out} register before month
    DWORD RegisterBeforeDay;   // {out} register before day
    DWORD EncryptedConstant;   // {in} Encryption Constant - should be extracted from project file
    bool EncryptedSections[NUMBER_OF_CRYPTED_SECTIONS]; // {out} Crypted sections
    char* PublicKey;           // {in} Public Key - should be extracted from project file
} TKeyVerifyParamsA, *PKeyVerifyParamsA;

typedef struct _TKeyVerifyParamsW
{
    wchar_t* RegInfo;             // {in} registration info/name
    DWORD KeyMode;             // {in} key generation mode (~RSA ???)
    DWORD KeyBase;             // {in} key output base (Base ???)
    wchar_t* Key;                 // {in} buffer for registration key
    DWORD KeyLen;              // {in} registration key size
    DWORD CreationYear;        // {out} key creation year
    DWORD CreationMonth;       // {out} key creation month
    DWORD CreationDay;         // {out} key creation day
    bool UseKeyExpiration;     // {out} has key expiration date?
    DWORD ExpirationYear;      // {out} key expiration year
    DWORD ExpirationMonth;     // {out} key expiration month
    DWORD ExpirationDay;       // {out} key expiration day
    bool UseHardwareLocking;   // {in} hardware locked key
    wchar_t* HardwareID;          // {in} pointer to null terminated hardware string
    bool UseExecutionsLimit;   // {out} limit key by executions?
    DWORD ExecutionsCount;     // {out} number of executions
    bool UseDaysLimit;         // {out} limit key by days?
    DWORD DaysCount;           // {out} number of days
    bool UseRunTimeLimit;      // {out} limit key by run time?
    DWORD RunTimeMinutes;      // {out} run time minutes
    bool UseGlobalTimeLimit;   // {out} limit key by global time?
    DWORD GlobalTimeMinutes;   // {out} global time minutes
    bool UseCountyLimit;       // {out} limit key by country?
    DWORD CountryCode;         // {out} country code
    bool UseRegisterAfter;     // {out} register key after date?
    DWORD RegisterAfterYear;   // {out} register after year
    DWORD RegisterAfterMonth;  // {out} register after month
    DWORD RegisterAfterDay;    // {out} register after day
    bool UseRegisterBefore;    // {out} register key before date?
    DWORD RegisterBeforeYear;  // {out} register before year
    DWORD RegisterBeforeMonth; // {out} register before month
    DWORD RegisterBeforeDay;   // {out} register before day
    DWORD EncryptedConstant;   // {in} Encryption Constant - should be extracted from project file
    bool EncryptedSections[NUMBER_OF_CRYPTED_SECTIONS]; // {out} Crypted sections
    wchar_t* PublicKey;           // {in} Public Key - should be extracted from project file
} TKeyVerifyParamsW, *PKeyVerifyParamsW;

typedef struct _TKeyGenParams
{
    DWORD KeyMode;             // {in} key generation mode (~RSA ???)
    DWORD KeyBase;             // {in} key output base (Base ???)
    bool KeyWithHyphens;       // {in} add hyphens to the key?
    char* Key;                 // {in} buffer for registration key
    DWORD KeyLen;              // {in} registration key size
    char* RegInfo;             // {in} registration info/name
    DWORD RegInfoLen;          // {in} registration info/name size
    bool UseKeyExpiration;     // {in} use key expiration?
    DWORD ExpirationYear;      // {in} key expiration year
    DWORD ExpirationMonth;     // {in} key expiration month
    DWORD ExpirationDay;       // {in} key expiration day
    bool UseHardwareLocking;   // {in} is key hardware locked?
    char* HardwareID;          // {in} pointer to null terminated hardware string
    bool UseExecutionsLimit;   // {in} limit key by executions?
    DWORD ExecutionsCount;     // {in} number of executions
    bool UseDaysLimit;         // {in} limit key by days?
    DWORD DaysCount;           // {in} number of days
    bool UseRunTimeLimit;      // {in} limit key by run time?
    DWORD RunTimeMinutes;      // {in} run time minutes
    bool UseGlobalTimeLimit;   // {in} limit key by global time?
    DWORD GlobalTimeMinutes;   // {in} global time minutes
    bool UseCountyLimit;       // {in} limit key by country?
    DWORD CountryCode;         // {in} country code
    bool UseRegisterAfter;     // {in} register key after date?
    DWORD RegisterAfterYear;   // {in} register after year
    DWORD RegisterAfterMonth;  // {in} register after month
    DWORD RegisterAfterDay;    // {in} register after day
    bool UseRegisterBefore;    // {in} register key before date?
    DWORD RegisterBeforeYear;  // {in} register before year
    DWORD RegisterBeforeMonth; // {in} register before month
    DWORD RegisterBeforeDay;   // {in} register before day
    DWORD EncryptedConstant;   // {in} Encryption Constant - should be extracted from project file
    bool  EncryptedSections[NUMBER_OF_CRYPTED_SECTIONS]; // {in} Crypted sections
    char* PrivateKey;          // {in} Private Key - should be extracted from project file
    char* PublicKey;           // {in} Public Key - should be extracted from project file
} TKeyGenParams, *PKeyGenParams;

typedef struct _TKeyGenParamsA
{
    DWORD KeyMode;             // {in} key generation mode (~RSA ???)
    DWORD KeyBase;             // {in} key output base (Base ???)
    bool KeyWithHyphens;       // {in} add hyphens to the key?
    char* Key;                 // {in} buffer for registration key
    DWORD KeyLen;              // {in} registration key size
    char* RegInfo;             // {in} registration info/name
    bool UseKeyExpiration;     // {in} use key expiration?
    DWORD ExpirationYear;      // {in} key expiration year
    DWORD ExpirationMonth;     // {in} key expiration month
    DWORD ExpirationDay;       // {in} key expiration day
    bool UseHardwareLocking;   // {in} is key hardware locked?
    char* HardwareID;          // {in} pointer to null terminated hardware string
    bool UseExecutionsLimit;   // {in} limit key by executions?
    DWORD ExecutionsCount;     // {in} number of executions
    bool UseDaysLimit;         // {in} limit key by days?
    DWORD DaysCount;           // {in} number of days
    bool UseRunTimeLimit;      // {in} limit key by run time?
    DWORD RunTimeMinutes;      // {in} run time minutes
    bool UseGlobalTimeLimit;   // {in} limit key by global time?
    DWORD GlobalTimeMinutes;   // {in} global time minutes
    bool UseCountyLimit;       // {in} limit key by country?
    DWORD CountryCode;         // {in} country code
    bool UseRegisterAfter;     // {in} register key after date?
    DWORD RegisterAfterYear;   // {in} register after year
    DWORD RegisterAfterMonth;  // {in} register after month
    DWORD RegisterAfterDay;    // {in} register after day
    bool UseRegisterBefore;    // {in} register key before date?
    DWORD RegisterBeforeYear;  // {in} register before year
    DWORD RegisterBeforeMonth; // {in} register before month
    DWORD RegisterBeforeDay;   // {in} register before day
    DWORD EncryptedConstant;   // {in} Encryption Constant - should be extracted from project file
    bool  EncryptedSections[NUMBER_OF_CRYPTED_SECTIONS]; // {in} Crypted sections
    char* PrivateKey;          // {in} Private Key - should be extracted from project file
    char* PublicKey;           // {in} Public Key - should be extracted from project file
} TKeyGenParamsA, *PKeyGenParamsA;

typedef struct _TKeyGenParamsW
{
    DWORD KeyMode;             // {in} key generation mode (~RSA ???)
    DWORD KeyBase;             // {in} key output base (Base ???)
    bool KeyWithHyphens;       // {in} add hyphens to the key?
    wchar_t* Key;                 // {in} buffer for registration key
    DWORD KeyLen;              // {in} registration key size
    wchar_t* RegInfo;             // {in} registration info/name
    bool UseKeyExpiration;     // {in} use key expiration?
    DWORD ExpirationYear;      // {in} key expiration year
    DWORD ExpirationMonth;     // {in} key expiration month
    DWORD ExpirationDay;       // {in} key expiration day
    bool UseHardwareLocking;   // {in} is key hardware locked?
    wchar_t* HardwareID;          // {in} pointer to null terminated hardware string
    bool UseExecutionsLimit;   // {in} limit key by executions?
    DWORD ExecutionsCount;     // {in} number of executions
    bool UseDaysLimit;         // {in} limit key by days?
    DWORD DaysCount;           // {in} number of days
    bool UseRunTimeLimit;      // {in} limit key by run time?
    DWORD RunTimeMinutes;      // {in} run time minutes
    bool UseGlobalTimeLimit;   // {in} limit key by global time?
    DWORD GlobalTimeMinutes;   // {in} global time minutes
    bool UseCountyLimit;       // {in} limit key by country?
    DWORD CountryCode;         // {in} country code
    bool UseRegisterAfter;     // {in} register key after date?
    DWORD RegisterAfterYear;   // {in} register after year
    DWORD RegisterAfterMonth;  // {in} register after month
    DWORD RegisterAfterDay;    // {in} register after day
    bool UseRegisterBefore;    // {in} register key before date?
    DWORD RegisterBeforeYear;  // {in} register before year
    DWORD RegisterBeforeMonth; // {in} register before month
    DWORD RegisterBeforeDay;   // {in} register before day
    DWORD EncryptedConstant;   // {in} Encryption Constant - should be extracted from project file
    bool  EncryptedSections[NUMBER_OF_CRYPTED_SECTIONS]; // {in} Crypted sections
    wchar_t* PrivateKey;          // {in} Private Key - should be extracted from project file
    wchar_t* PublicKey;           // {in} Public Key - should be extracted from project file
} TKeyGenParamsW, *PKeyGenParamsW;

 /* KeyGen APIs */
 extern  "C" __declspec( dllimport ) __stdcall int KG_GenerateRegistrationKey(PKeyGenParams kg);
 extern  "C" __declspec( dllimport ) __stdcall int KG_GenerateRegistrationKeyA(PKeyGenParamsA kg);
 extern  "C" __declspec( dllimport ) __stdcall int KG_GenerateRegistrationKeyW(PKeyGenParamsW kg);
 extern  "C" __declspec( dllimport ) __stdcall int KG_GenerateRegistrationKeyFromProject(char* pfile, PKeyGenParams kg);
 extern  "C" __declspec( dllimport ) __stdcall int KG_GenerateRegistrationKeyFromProjectA(char* pfile, PKeyGenParamsA kg);
 extern  "C" __declspec( dllimport ) __stdcall int KG_GenerateRegistrationKeyFromProjectW(wchar_t* pfile, PKeyGenParamsW kg);
 extern  "C" __declspec( dllimport ) __stdcall int KG_VerifyRegistrationInfo(PKeyVerifyParams kv);
 extern  "C" __declspec( dllimport ) __stdcall int KG_VerifyRegistrationInfoA(PKeyVerifyParamsA kv);
 extern  "C" __declspec( dllimport ) __stdcall int KG_VerifyRegistrationInfoW(PKeyVerifyParamsW kv);
 extern  "C" __declspec( dllimport ) __stdcall int KG_VerifyRegistrationInfoFromProject(char* pfile, PKeyVerifyParams kv);
 extern  "C" __declspec( dllimport ) __stdcall int KG_VerifyRegistrationInfoFromProjectA(char* pfile, PKeyVerifyParamsA kv);
 extern  "C" __declspec( dllimport ) __stdcall int KG_VerifyRegistrationInfoFromProjectW(wchar_t* pfile, PKeyVerifyParamsW kv);
