#include <catch.hpp>
#include <libdane/Certificate.h>
#include "../resources.h"

using namespace libdane;

SCENARIO("Accessors work")
{
	GIVEN("The certificate for google.com")
	{
		Certificate cert(resources::googlePEM);
		
		THEN("The certificate should be truthy")
		{
			CHECK(!!cert);
		}
		
		THEN("Issuer and Subject should be correct")
		{
			CHECK(cert.issuerDN() == "/C=US/O=Google Inc/CN=Google Internet Authority G2");
			CHECK(cert.subjectDN() == "/C=US/ST=California/L=Mountain View/O=Google Inc/CN=*.google.com");
		}
		
		THEN("Data blobs should be correct")
		{
			CHECK(cert.encoded().sha256().hex() == "440e875366ad14f997127cb7199dd255fc01fb1c57a7adde9415dca228535404");
			CHECK(cert.publicKey().sha256().hex() == "7b1394143f8d123760572cc410d1441b3bea3aaf987d0e4b950749e72d554db9");
		}
		
		THEN("Selection should return the correct blob")
		{
			CHECK(cert.select(libdane::FullCertificate) == cert.encoded());
			CHECK(cert.select(libdane::SubjectPublicKeyInfo) == cert.publicKey());
		}
	}
	
	GIVEN("An empty certificate")
	{
		Certificate cert;
		
		THEN("The certificate should be falsy")
		{
			CHECK(!cert);
		}
		
		THEN("Issuer and Subject should be blank")
		{
			CHECK(cert.issuerDN() == "");
			CHECK(cert.subjectDN() == "");
		}
	}
}

SCENARIO("PEM parsing works")
{
	GIVEN("The certificate chain for google.com")
	{
		std::deque<Certificate> certs = Certificate::parsePEM(resources::googlePEM);
		
		THEN("The size should be correct")
		{
			REQUIRE(certs.size() == 3);
		}
		
		THEN("The certificates in it should be correct")
		{
			CHECK(certs[0].subjectDN() == "/C=US/ST=California/L=Mountain View/O=Google Inc/CN=*.google.com");
			CHECK(certs[1].subjectDN() == "/C=US/O=Google Inc/CN=Google Internet Authority G2");
			CHECK(certs[2].subjectDN() == "/C=US/O=GeoTrust Inc./CN=GeoTrust Global CA");
		}
	}
}
