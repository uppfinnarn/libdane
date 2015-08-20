#include <catch.hpp>
#include <libdane/DANERecord.h>
#include <libdane/Certificate.h>
#include "../resources.h"

using namespace libdane;

SCENARIO("Lone certificates can be verified")
{
	Certificate cert = Certificate::parsePEM(resources::googlePEM).front();
	DANERecord rec(DomainIssuedCertificate, FullCertificate, ExactMatch, cert.encoded());
	
	WHEN("Different selectors are used")
	{
		GIVEN("Full Certificate")
		{
			rec.setSelector(FullCertificate);
			rec.setData(cert.encoded());
			CHECK(rec.verify(cert));
		}
		
		GIVEN("Public Key")
		{
			rec.setSelector(SubjectPublicKeyInfo);
			rec.setData(cert.publicKey());
			CHECK(rec.verify(cert));
		}
	}
	
	WHEN("Different matching types are used")
	{
		GIVEN("SHA256Hash")
		{
			rec.setMatching(SHA256Hash);
			rec.setData(rec.data().sha256());
			CHECK(rec.verify(cert));
		}
		
		GIVEN("SHA512Hash")
		{
			rec.setMatching(SHA512Hash);
			rec.setData(rec.data().sha512());
			CHECK(rec.verify(cert));
		}
	}
}

SCENARIO("Certificate chains can be verified")
{
	GIVEN("The certificate chain for google.com")
	{
		std::deque<Certificate> chain = Certificate::parsePEM(resources::googlePEM);
		const Certificate &cert = chain.back();
		
		GIVEN("A passing DomainIssuedCertificate record")
		{
			DANERecord rec(DomainIssuedCertificate, FullCertificate, SHA256Hash, chain.front().select(FullCertificate).match(SHA256Hash));
			
			THEN("Verification should succeed")
			{
				CHECK(rec.verify(true, cert, chain) == DANERecord::PassAll);
			}
			
			THEN("Preverification should be ignored")
			{
				CHECK(rec.verify(false, cert, chain) == DANERecord::PassAll);
			}
		}
		
		GIVEN("A failing DomainIssuedCertificate record")
		{
			// Note the mismatched hash algorithm
			DANERecord rec(DomainIssuedCertificate, FullCertificate, SHA256Hash, chain.front().select(FullCertificate).match(SHA512Hash));
			
			THEN("Verification should fail")
			{
				CHECK(rec.verify(false, cert, chain) == DANERecord::Fail);
			}
		}
	}
}