/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFile.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:48 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/01 19:54:36 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADEDFILE_HPP
#define UPLOADEDFILE_HPP

#include <string>
#include <vector>
#include "clientRequest.hpp"

struct UploadedFile {
	std::string filename;
	std::string contentType;
	std::string fieldName;
	std::vector<char> content;
	size_t size;

	UploadedFile() : size(0) {}
};

#endif
